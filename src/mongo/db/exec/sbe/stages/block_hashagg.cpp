/**
 *    Copyright (C) 2024-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/db/exec/sbe/values/block_interface.h"
#include "mongo/db/exec/sbe/values/value.h"
#include "mongo/platform/basic.h"

#include "mongo/db/exec/sbe/expressions/compile_ctx.h"
#include "mongo/db/exec/sbe/stages/block_hashagg.h"
#include "mongo/db/exec/sbe/util/spilling.h"
#include "mongo/db/storage/kv/kv_engine.h"
#include "mongo/db/storage/storage_engine.h"
#include "mongo/util/assert_util.h"
#include "mongo/util/str.h"

#include "mongo/db/exec/sbe/size_estimator.h"

namespace mongo {
namespace sbe {
namespace {
bool allFalse(value::ValueBlock* bitset) {
    const auto& deblocked = bitset->extract();
    for (size_t i = 0; i < deblocked.count; i++) {
        invariant(deblocked[i].first == value::TypeTags::Boolean);
        if (value::bitcastTo<bool>(deblocked[i].second)) {
            return false;
        }
    }
    return true;
}
}  // namespace

BlockHashAggStage::BlockHashAggStage(std::unique_ptr<PlanStage> input,
                                     value::SlotId groupSlotId,
                                     value::SlotId blockBitsetSlotId,
                                     value::SlotId rowAccSlotId,
                                     BlockAndRowAggs aggs,
                                     PlanNodeId planNodeId,
                                     bool participateInTrialRunTracking)
    : PlanStage("block_hashagg"_sd, planNodeId, participateInTrialRunTracking),
      _groupSlot(groupSlotId),
      _blockBitsetSlotId(blockBitsetSlotId),
      _rowAccSlotId(rowAccSlotId),
      _blockRowAggs(std::move(aggs)) {
    _children.emplace_back(std::move(input));
    _outAggBlocks.resize(_blockRowAggs.size());
}

std::unique_ptr<PlanStage> BlockHashAggStage::clone() const {
    BlockAndRowAggs blockRowAggs;
    for (const auto& [slot, aggs] : _blockRowAggs) {
        blockRowAggs.emplace(slot,
                             BlockRowAccumulators{aggs.blockAgg->clone(), aggs.rowAgg->clone()});
    }

    return std::make_unique<BlockHashAggStage>(_children[0]->clone(),
                                               _groupSlot,
                                               _blockBitsetSlotId,
                                               _rowAccSlotId,
                                               std::move(blockRowAggs),
                                               _commonStats.nodeId,
                                               _participateInTrialRunTracking);
}

void BlockHashAggStage::doSaveState(bool relinquishCursor) {}

void BlockHashAggStage::doRestoreState(bool relinquishCursor) {}

void BlockHashAggStage::doDetachFromOperationContext() {}

void BlockHashAggStage::doAttachToOperationContext(OperationContext* opCtx) {}

void BlockHashAggStage::prepare(CompileCtx& ctx) {
    _children[0]->prepare(ctx);

    _idHtAccessor.emplace(_htIt, 0);
    _idAccessorIn = _children[0]->getAccessor(ctx, _groupSlot);
    invariant(_idAccessorIn);
    _blockBitsetAccessorIn = _children[0]->getAccessor(ctx, _blockBitsetSlotId);
    invariant(_blockBitsetAccessorIn);

    value::SlotSet dupCheck;
    auto throwIfDupSlot = [&dupCheck](value::SlotId slot) {
        auto [_, inserted] = dupCheck.emplace(slot);
        tassert(7953400, "duplicate slot id", inserted);
    };

    _outAccessorsMap.reserve(_blockRowAggs.size() + 1);
    throwIfDupSlot(_groupSlot);
    throwIfDupSlot(_blockBitsetSlotId);
    _outAccessorsMap[_groupSlot] = &_outIdBlockAccessor;

    _outAggBlockAccessors.resize(_blockRowAggs.size());
    // Change the agg slot accessors to point to the blocks.
    size_t i = 0;
    for (auto& outBlock : _outAggBlocks) {
        _outAggBlockAccessors[i].reset(
            false, value::TypeTags::valueBlock, value::bitcastFrom<value::ValueBlock*>(&outBlock));
        ++i;
    }

    i = 0;
    for (auto& [slot, aggs] : _blockRowAggs) {
        throwIfDupSlot(slot);

        _rowAggHtAccessors.emplace_back(std::make_unique<HashAggAccessor>(_htIt, i));
        _outAccessorsMap[slot] = &_outAggBlockAccessors[i];

        ctx.root = this;
        ctx.aggExpression = true;
        ctx.accumulator = _rowAggHtAccessors.back().get();
        _aggCodes.emplace_back(aggs.rowAgg->compile(ctx));
        ctx.aggExpression = false;

        // Also compile the block level agg.
        _blockLevelAggCodes.emplace_back(aggs.blockAgg->compile(ctx));
        ++i;
    }
    _compiled = true;
}

value::SlotAccessor* BlockHashAggStage::getAccessor(CompileCtx& ctx, value::SlotId slot) {
    if (_compiled && _outAccessorsMap.count(slot)) {
        return _outAccessorsMap[slot];
    } else {
        if (_rowAccSlotId == slot) {
            return &_internalAccessor;
        }
        return _children[0]->getAccessor(ctx, slot);
    }
}

void BlockHashAggStage::open(bool reOpen) {
    auto optTimer(getOptTimer(_opCtx));
    _children[0]->open(reOpen);
    _commonStats.opens++;

    while (PlanState::ADVANCED == _children[0]->getNext()) {
        auto [bitsetTag, bitsetVal] = _blockBitsetAccessorIn->getViewOfValue();
        invariant(bitsetTag == value::TypeTags::valueBlock);
        auto bitset = value::bitcastTo<value::ValueBlock*>(bitsetVal);
        // If all bits are false, there's no work to do. We don't want to make an erroneous entry in
        // our hash map.
        if (allFalse(bitset)) {
            continue;
        }

        auto [gbTag, gbVal] = _idAccessorIn->getViewOfValue();
        // TODO SERVER-84685 handle groupby key inputs in the form of blocks.
        invariant(gbTag != value::TypeTags::valueBlock);
        value::MaterializedRow key{1};
        key.reset(0, false, gbTag, gbVal);

        _htIt = _ht.find(key);
        if (_htIt == _ht.end()) {
            // New key we haven't seen before.
            key.makeOwned();
            auto [it, _] = _ht.emplace(std::move(key), value::MaterializedRow{0});
            // Initialize accumulators.
            it->second.resize(_rowAggHtAccessors.size());
            _htIt = it;
        }

        // Now run the block level accumulators followed by the row level accumulators.
        size_t i = 0;
        for (auto& blockAccum : _blockLevelAggCodes) {
            auto [owned, tag, val] = _bytecode.run(blockAccum.get());

            // Now run the row level accumulator.
            _internalAccessor.reset(owned, tag, val);
            auto [rowOwned, rowTag, rowVal] = _bytecode.run(_aggCodes[i].get());
            _rowAggHtAccessors[i]->reset(rowOwned, rowTag, rowVal);
            ++i;
        }

        if (_tracker && _tracker->trackProgress<TrialRunTracker::kNumResults>(1)) {
            // During trial runs, we want to limit the amount of work done by opening a blocking
            // stage, like this one. The blocking stage tracks the number of documents it has
            // read from its child, and if the TrialRunTracker ends the trial, a special
            // exception returns control back to the planner.
            _tracker = nullptr;
            _children[0]->close();
            uasserted(ErrorCodes::QueryTrialRunCompleted, "Trial run early exit in group");
        }
    }

    _htIt = _ht.end();
}

PlanState BlockHashAggStage::getNext() {
    auto optTimer(getOptTimer(_opCtx));

    if (_htIt == _ht.end()) {
        _htIt = _ht.begin();
    } else {
        ++_htIt;
    }

    if (_htIt == _ht.end()) {
        return trackPlanState(PlanState::IS_EOF);
    }
    // TODO SERVER-85537: Right now this is just returning blocks of size 1. We should have this
    // return larger blocks for following BP stages.
    const size_t kBlockOutSize = 1;
    _outIdBlock.clear();
    _outIdBlock.reserve(kBlockOutSize);
    for (auto& b : _outAggBlocks) {
        b.clear();
        b.reserve(kBlockOutSize);
    }
    invariant(_outAggBlocks.size() == _outAggBlockAccessors.size());
    invariant(_outAggBlocks.size() == _rowAggHtAccessors.size());

    // Copy the key from the current element in the HT into the out block.
    auto [t, v] = _idHtAccessor->copyOrMoveValue();
    _outIdBlock.push_back(t, v);
    _outIdBlockAccessor.reset(
        false, value::TypeTags::valueBlock, value::bitcastFrom<value::ValueBlock*>(&_outIdBlock));

    // Copy the values from the current element in the HT into the out block.
    size_t i = 0;
    for (auto& acc : _rowAggHtAccessors) {
        auto [t, v] = acc->copyOrMoveValue();
        _outAggBlocks[i].push_back(t, v);
        ++i;
    }

    return trackPlanState(PlanState::ADVANCED);
}

std::unique_ptr<PlanStageStats> BlockHashAggStage::getStats(bool includeDebugInfo) const {
    auto ret = std::make_unique<PlanStageStats>(_commonStats);
    ret->specific = std::make_unique<HashAggStats>(_specificStats);

    if (includeDebugInfo) {
        DebugPrinter printer;
        BSONObjBuilder bob;
        bob.append("groupBySlot", _groupSlot);
        bob.append("rowAccSlotId", _rowAccSlotId);

        BSONObjBuilder blockExprBob(bob.subobjStart("blockExpressions"));
        for (auto&& [slot, expr] : _blockRowAggs) {
            blockExprBob.append(str::stream() << slot, printer.print(expr.blockAgg->debugPrint()));
        }

        BSONObjBuilder rowExprBob(bob.subobjStart("rowExpressions"));
        for (auto&& [slot, expr] : _blockRowAggs) {
            rowExprBob.append(str::stream() << slot, printer.print(expr.rowAgg->debugPrint()));
        }

        ret->debugInfo = bob.obj();
    }

    ret->children.emplace_back(_children[0]->getStats(includeDebugInfo));
    return ret;
}

const SpecificStats* BlockHashAggStage::getSpecificStats() const {
    return &_specificStats;
}

void BlockHashAggStage::close() {
    auto optTimer(getOptTimer(_opCtx));

    trackClose();
    _children[0]->close();
}

std::vector<DebugPrinter::Block> BlockHashAggStage::debugPrint() const {
    auto ret = PlanStage::debugPrint();

    ret.emplace_back(DebugPrinter::Block("[`"));
    DebugPrinter::addIdentifier(ret, _groupSlot);
    ret.emplace_back(DebugPrinter::Block("`]"));

    // Print the row-level and block-level accessors.
    for (bool blockExprPrint : {true, false}) {
        ret.emplace_back(DebugPrinter::Block("[`"));
        bool first = true;
        for (auto&& [slot, expr] : _blockRowAggs) {
            if (!first) {
                ret.emplace_back(DebugPrinter::Block("`,"));
            }

            DebugPrinter::addIdentifier(ret, slot);
            ret.emplace_back("=");
            const auto& aggExpr = blockExprPrint ? expr.blockAgg : expr.rowAgg;
            DebugPrinter::addBlocks(ret, aggExpr->debugPrint());
            first = false;
        }
        ret.emplace_back("`]");
    }

    ret.emplace_back(DebugPrinter::Block("[`"));
    DebugPrinter::addIdentifier(ret, _rowAccSlotId);
    ret.emplace_back(DebugPrinter::Block("`]"));

    DebugPrinter::addNewLine(ret);
    DebugPrinter::addBlocks(ret, _children[0]->debugPrint());

    return ret;
}

size_t BlockHashAggStage::estimateCompileTimeSize() const {
    size_t size = sizeof(*this);
    size += size_estimator::estimate(_children);
    size += size_estimator::estimate(_blockRowAggs);
    size += size_estimator::estimate(_outAccessorsMap);
    return size;
}

void BlockHashAggStage::doDetachFromTrialRunTracker() {
    _tracker = nullptr;
}

PlanStage::TrialRunTrackerAttachResultMask BlockHashAggStage::doAttachToTrialRunTracker(
    TrialRunTracker* tracker, TrialRunTrackerAttachResultMask childrenAttachResult) {
    // The BlockHashAggStage only tracks the "numResults" metric when it is the most deeply nested
    // blocking stage.
    if (!(childrenAttachResult & TrialRunTrackerAttachResultFlags::AttachedToBlockingStage)) {
        _tracker = tracker;
    }

    // Return true to indicate that the tracker is attached to a blocking stage: either this stage
    // or one of its descendent stages.
    return childrenAttachResult | TrialRunTrackerAttachResultFlags::AttachedToBlockingStage;
}

}  // namespace sbe
}  // namespace mongo
