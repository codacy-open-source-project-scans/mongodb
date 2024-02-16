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

#include "mongo/bson/json.h"
#include "mongo/db/catalog/catalog_test_fixture.h"
#include "mongo/db/catalog/collection_options.h"
#include "mongo/db/index/index_descriptor.h"
#include "mongo/db/pipeline/document_source_internal_projection.h"
#include "mongo/db/pipeline/expression_context_for_test.h"
#include "mongo/db/query/canonical_query.h"
#include "mongo/db/query/classic_runtime_planner_for_sbe/planner_interface.h"
#include "mongo/db/query/index_entry.h"
#include "mongo/db/query/multiple_collection_accessor.h"
#include "mongo/db/query/plan_cache_key_factory.h"
#include "mongo/db/query/query_solution.h"
#include "mongo/db/shard_role.h"

namespace mongo::classic_runtime_planner_for_sbe {
namespace {

const NamespaceString kNss = NamespaceString::createNamespaceString_forTest("test.collection");
const BSONObj kFindFilter = fromjson("{a: {$gte: 0}, b: {$gte: 0}}");
const BSONObj kAddFieldsSpec = fromjson(R"({sum: {$add: ["$a", "$b"]}})");
/**
 * Fixture for classic_runtime_planner_for_sbe::PlannerInterface implementations. As a test query,
 * it uses an aggregation pipeline [{$match: <match statement>}, {$addFields: <addFields statement>]
 * where $match is pushed down to the find query and $addFields is left to be a single-stage agg
 * pipeline.
 */
class ClassicRuntimePlannerForSbeTest : public CatalogTestFixture {
protected:
    void setUp() final {
        CatalogTestFixture::setUp();
        OperationContext* opCtx = operationContext();
        expCtx = make_intrusive<ExpressionContextForTest>(opCtx, kNss);

        ASSERT_OK(storageInterface()->createCollection(opCtx, kNss, CollectionOptions{}));
    }

    void tearDown() final {
        _collections.reset();
        expCtx.reset();
        CatalogTestFixture::tearDown();
    }

    /**
     * Creates PlannerData for the following pipeline: [
     *   {$match: 'findFilter'}},
     *   {$addFields: 'addFieldsSpec'}}
     * ]
     * Defaults to kFindFilter and kAddFieldsSpec for 'findFilter' and 'addFieldsSpec' respectively
     * if arguments are not supplied.
     */
    PlannerData createPlannerData(BSONObj findFilter = kFindFilter,
                                  BSONObj addFieldsSpec = kAddFieldsSpec) {
        auto findCommand = std::make_unique<FindCommandRequest>(kNss);
        findCommand->setFilter(findFilter);

        auto cq = std::make_unique<CanonicalQuery>(CanonicalQueryParams{
            .expCtx = expCtx,
            .parsedFind = ParsedFindCommandParams{.findCommand = std::move(findCommand)},
            .pipeline = {make_intrusive<DocumentSourceInternalProjection>(
                expCtx, addFieldsSpec, InternalProjectionPolicyEnum::kAddFields)}});
        cq->setSbeCompatible(true);

        auto opCtx = operationContext();
        _collections.emplace(acquireCollectionMaybeLockFree(
            opCtx,
            CollectionAcquisitionRequest::fromOpCtx(opCtx, kNss, AcquisitionPrerequisites::kRead)));

        return {
            .cq = std::move(cq),
            .sbeYieldPolicy = makeYieldPolicy(),
            .workingSet = std::make_unique<WorkingSet>(),
            .collections = *_collections,
            .plannerParams = _params,
            .cachedPlanHash = boost::none,
        };
    }

    /**
     * For a given PlanExecutor, asserts the following:
     * 1. PlanExecutor returns EOF after exactly expectedSums.size() documents.
     * 2. Each returned document have a field "sum" with an integer value, equal to corresponding
     *    element in expectedSums.
     */
    void assertPlanExecutorReturnsCorrectSums(std::vector<int> expectedSums, PlanExecutor* exec) {
        for (int expectedSum : expectedSums) {
            BSONObj out;
            auto state = exec->getNext(&out, nullptr);
            ASSERT_EQ(PlanExecutor::ExecState::ADVANCED, state);
            ASSERT_EQ(expectedSum, out.getField("sum").Int());
        }
        ASSERT_EQ(PlanExecutor::ExecState::IS_EOF, exec->getNextDocument(nullptr, nullptr));
    }

    /**
     * Create an 'index' on an empty collection with name 'indexName' and add the index to
     * QueryPlannerParams.
     */
    void createIndexOnEmptyCollection(OperationContext* opCtx,
                                      BSONObj index,
                                      std::string indexName) {
        auto acquisition = acquireCollection(
            opCtx,
            CollectionAcquisitionRequest::fromOpCtx(opCtx, kNss, AcquisitionPrerequisites::kWrite),
            MODE_X);
        CollectionWriter coll(opCtx, &acquisition);

        WriteUnitOfWork wunit(opCtx);
        auto indexCatalog = coll.getWritableCollection(opCtx)->getIndexCatalog();
        ASSERT(indexCatalog);
        auto indexesBefore = indexCatalog->numIndexesReady();
        ASSERT_OK(indexCatalog
                      ->createIndexOnEmptyCollection(
                          opCtx, coll.getWritableCollection(opCtx), makeIndexSpec(index, indexName))
                      .getStatus());
        wunit.commit();
        ASSERT_EQ(indexesBefore + 1, indexCatalog->numIndexesReady());

        // The QueryPlannerParams should also have information about the index to consider it when
        // actually doing the planning.
        _params.indices.push_back(
            IndexEntry(index,
                       IndexNames::nameToType(IndexNames::findPluginName(index)),
                       IndexDescriptor::kLatestIndexVersion,
                       false,
                       {},
                       {},
                       false,
                       false,
                       IndexEntry::Identifier{indexName},
                       nullptr,
                       BSONObj(),
                       nullptr,
                       nullptr));
    }

    BSONObj makeIndexSpec(BSONObj index, StringData indexName) {
        return BSON("v" << IndexDescriptor::kLatestIndexVersion << "key" << index << "name"
                        << indexName);
    }

    // Creates indexes {a: 1} and {b: 1}, inserts 100 docs with {_id: i, a: i, b: i}, creates the
    // subplanner with a rooted or filter and returns the PlanExecutor.
    std::unique_ptr<PlanExecutor, PlanExecutor::Deleter> getExecutorWithSubPlanning() {
        auto opCtx = operationContext();

        createIndexOnEmptyCollection(opCtx, BSON("a" << 1), "a_1");
        createIndexOnEmptyCollection(opCtx, BSON("b" << 1), "b_1");

        std::vector<InsertStatement> docs;
        for (int i = 0; i < 100; ++i) {
            docs.emplace_back(InsertStatement(BSON("_id" << i << "a" << i << "b" << i)));
        }

        ASSERT_OK(storageInterface()->insertDocuments(opCtx, kNss, docs));

        // createPlannerData adds a cqPipeline with an $addFields stage.
        SubPlanner planner{
            opCtx,
            createPlannerData(fromjson(
                "{$or: [{a: {$lte: 10}, b: {$gte: 10}}, {a: {$lte: 90}, b: {$gte: 90}}]}")),
            PlanYieldPolicy::YieldPolicy::INTERRUPT_ONLY};

        auto exec = planner.plan();
        assertPlanExecutorReturnsCorrectSums({20, 180}, exec.get());
        return exec;
    }

    boost::intrusive_ptr<ExpressionContext> expCtx;

private:
    std::unique_ptr<PlanYieldPolicySBE> makeYieldPolicy() {
        return PlanYieldPolicySBE::make(
            operationContext(),
            PlanYieldPolicy::YieldPolicy::YIELD_AUTO,
            operationContext()->getServiceContext()->getFastClockSource(),
            0,
            Milliseconds::zero(),
            PlanYieldPolicy::YieldThroughAcquisitions{});
    }

    boost::optional<MultipleCollectionAccessor> _collections;
    QueryPlannerParams _params{QueryPlannerParams::DEFAULT};
};

TEST_F(ClassicRuntimePlannerForSbeTest, SingleSolutionPassthroughPlannerCreatesCacheEntry) {
    static const std::vector<BSONArray> kDocs = {
        BSON_ARRAY(int64_t{0} << BSON("a" << 1 << "b" << 2)),
        BSON_ARRAY(int64_t{1} << BSON("a" << 2 << "b" << 2)),
        BSON_ARRAY(int64_t{2} << BSON("a" << 3 << "b" << 2))};

    {  // Run SingleSolutionPassthroughPlanner to create pinned cached entry.
        auto root = std::make_unique<VirtualScanNode>(
            kDocs, VirtualScanNode::ScanType::kIxscan, true /*hasRecordId*/, BSON("a" << 1));
        auto solution = std::make_unique<QuerySolution>();
        solution->setRoot(std::move(root));

        SingleSolutionPassthroughPlanner planner{
            operationContext(), createPlannerData(), std::move(solution)};
        auto exec = planner.plan();
        assertPlanExecutorReturnsCorrectSums({3, 4, 5}, exec.get());
    }

    {  // Run CachedPlanner to execute the cached plan.
        PlannerData plannerData = createPlannerData();
        auto planCacheKey =
            plan_cache_key_factory::make(*plannerData.cq,
                                         plannerData.collections,
                                         canonical_query_encoder::Optimizer::kSbeStageBuilders);
        auto&& planCache = sbe::getPlanCache(operationContext());
        auto cacheEntry = planCache.getCacheEntryIfActive(planCacheKey);
        ASSERT_TRUE(cacheEntry);
        CachedPlanner cachedPlanner{
            operationContext(), std::move(plannerData), std::move(cacheEntry)};

        auto cachedExec = cachedPlanner.plan();
        assertPlanExecutorReturnsCorrectSums({3, 4, 5}, cachedExec.get());
    }
}

TEST_F(ClassicRuntimePlannerForSbeTest, MultiPlannerPicksMoreEfficientPlan) {
    // Ensures that cache entries are available immediately.
    bool previousQueryKnobValue = internalQueryCacheDisableInactiveEntries.swap(true);
    ON_BLOCK_EXIT([&] { internalQueryCacheDisableInactiveEntries.store(previousQueryKnobValue); });

    PlannerData plannerData = createPlannerData();
    std::vector<std::unique_ptr<QuerySolution>> solutions;

    auto addVirtualScanSolution = [&](std::vector<BSONArray> docs) {
        auto virtScan = std::make_unique<VirtualScanNode>(
            std::move(docs), VirtualScanNode::ScanType::kCollScan, false /*hasRecordId*/);
        virtScan->filter = plannerData.cq->getPrimaryMatchExpression()->clone();
        auto solution = std::make_unique<QuerySolution>();
        solution->setRoot(std::move(virtScan));
        solution->cacheData = std::make_unique<SolutionCacheData>();
        solution->cacheData->solnType = SolutionCacheData::COLLSCAN_SOLN;
        solution->cacheData->tree = std::make_unique<PlanCacheIndexTree>();
        solutions.push_back(std::move(solution));
    };

    std::vector<int> expectedSums;
    expectedSums.reserve(200);
    {  // Generate a plan with 400 total documents and 200 documents matching the filter. We expect
       // the multi-planner to choose this plan.
        std::vector<BSONArray> docs;
        docs.reserve(400);
        for (int i = 1; i <= 200; ++i) {
            docs.push_back(BSON_ARRAY(BSON("a" << i << "b" << 1)));
            docs.push_back(BSON_ARRAY(BSON("a" << -i << "b" << 1)));
            expectedSums.push_back(i + 1);
        }
        addVirtualScanSolution(std::move(docs));
    }
    {  // Generate a plan with 600 total documents and 200 documents matching the filter.
        std::vector<BSONArray> docs;
        docs.reserve(600);
        for (int i = 1; i <= 200; ++i) {
            docs.push_back(BSON_ARRAY(BSON("a" << i << "b" << 2)));
            docs.push_back(BSON_ARRAY(BSON("a" << -i << "b" << 2)));
            docs.push_back(BSON_ARRAY(BSON("a" << -i << "b" << 2)));
        }
        addVirtualScanSolution(std::move(docs));
    }

    MultiPlanner planner{operationContext(),
                         std::move(plannerData),
                         PlanYieldPolicy::YieldPolicy::INTERRUPT_ONLY,
                         std::move(solutions)};
    auto exec = planner.plan();
    assertPlanExecutorReturnsCorrectSums(expectedSums, exec.get());

    {  // Run CachedPlanner to execute the cached plan.
        PlannerData plannerData = createPlannerData();
        auto planCacheKey =
            plan_cache_key_factory::make(*plannerData.cq,
                                         plannerData.collections,
                                         canonical_query_encoder::Optimizer::kSbeStageBuilders);
        auto&& planCache = sbe::getPlanCache(operationContext());
        auto cacheEntry = planCache.getCacheEntryIfActive(planCacheKey);
        ASSERT_TRUE(cacheEntry);
        CachedPlanner cachedPlanner{
            operationContext(), std::move(plannerData), std::move(cacheEntry)};
        auto cachedExec = cachedPlanner.plan();
        assertPlanExecutorReturnsCorrectSums(std::move(expectedSums), cachedExec.get());
    }
}

TEST_F(ClassicRuntimePlannerForSbeTest, SubPlannerPicksMoreEfficientPlanForEachBranch) {
    auto exec = getExecutorWithSubPlanning();
    PlanSummaryStats stats;
    exec->getPlanExplainer().getSummaryStats(&stats);

    // The most efficient solution should use index "a" for first branch, examining 10 keys (a: 1 to
    // a: 10) and index "b" for second branch, examining 11 keys (b: 90 to b: 100).
    ASSERT_EQ(2, stats.indexesUsed.size());
    ASSERT_EQ(21, stats.totalKeysExamined);
    ASSERT_TRUE(std::find(stats.indexesUsed.begin(), stats.indexesUsed.end(), "a_1") !=
                stats.indexesUsed.end());
    ASSERT_TRUE(std::find(stats.indexesUsed.begin(), stats.indexesUsed.end(), "b_1") !=
                stats.indexesUsed.end());
}

TEST_F(ClassicRuntimePlannerForSbeTest, SubPlannerPicksCachedPlanForWholeQuery) {
    auto exec = getExecutorWithSubPlanning();

    {  // Run CachedPlanner to execute the cached plan.
        PlannerData plannerData = createPlannerData(
            fromjson("{$or: [{a: {$lte: 10}, b: {$gte: 10}}, {a: {$lte: 90}, b: {$gte: 90}}]}"));
        auto planCacheKey =
            plan_cache_key_factory::make(*plannerData.cq,
                                         plannerData.collections,
                                         canonical_query_encoder::Optimizer::kSbeStageBuilders);
        auto&& planCache = sbe::getPlanCache(operationContext());
        auto cacheEntry = planCache.getCacheEntryIfActive(planCacheKey);
        ASSERT_TRUE(cacheEntry);
        CachedPlanner cachedPlanner{
            operationContext(), std::move(plannerData), std::move(cacheEntry)};

        auto cachedExec = cachedPlanner.plan();
        assertPlanExecutorReturnsCorrectSums({20, 180}, cachedExec.get());
    }
}

}  // namespace
}  // namespace mongo::classic_runtime_planner_for_sbe
