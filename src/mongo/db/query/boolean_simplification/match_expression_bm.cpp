/**
 *    Copyright (C) 2023-present MongoDB, Inc.
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

#include "mongo/db/matcher/expression_leaf.h"
#include "mongo/db/matcher/expression_tree.h"
#include <benchmark/benchmark.h>

namespace mongo::boolean_simplification {
/**
 * 1 Minterm of size N
 */
void matchExpression_createAnd(benchmark::State& state) {
    const size_t numPredicates = static_cast<size_t>(state.range());
    std::vector<BSONObj> operands;
    operands.reserve(numPredicates);
    for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
        operands.emplace_back(BSON("a" << static_cast<int>(predicateIndex)));
    }

    for (auto _ : state) {
        auto root = std::make_unique<AndMatchExpression>();

        for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
            root->add(
                std::make_unique<EqualityMatchExpression>("a"_sd, operands[predicateIndex]["a"]));
        }
    }
}

void matchExpression_createOr(benchmark::State& state) {
    const size_t numPredicates = static_cast<size_t>(state.range());
    std::vector<BSONObj> operands;
    operands.reserve(numPredicates);
    for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
        operands.emplace_back(BSON("a" << static_cast<int>(predicateIndex)));
    }

    for (auto _ : state) {
        auto root = std::make_unique<OrMatchExpression>();

        for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
            root->add(
                std::make_unique<EqualityMatchExpression>("a"_sd, operands[predicateIndex]["a"]));
        }
    }
}

void matchExpression_createAndOfOrs(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range());
    std::vector<BSONObj> operands;
    operands.reserve(size);
    for (size_t index = 0; index < size; ++index) {
        operands.emplace_back(BSON("a" << static_cast<int>(index)));
    }

    for (auto _ : state) {
        auto root = std::make_unique<AndMatchExpression>();
        for (size_t index = 0; index < size; ++index) {
            auto orExpr = std::make_unique<OrMatchExpression>();
            orExpr->add(std::make_unique<EqualityMatchExpression>("a"_sd, operands[index]["a"]));
            orExpr->add(std::make_unique<GTMatchExpression>("a"_sd, operands[index]["a"]));
            root->add(std::move(orExpr));
        }
    }
}

void matchExpression_cloneAnd(benchmark::State& state) {
    const size_t numPredicates = static_cast<size_t>(state.range());
    auto root = std::make_unique<AndMatchExpression>();
    std::vector<BSONObj> operands;
    operands.reserve(numPredicates);
    for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
        operands.emplace_back(BSON("a" << static_cast<int>(predicateIndex)));
        root->add(std::make_unique<EqualityMatchExpression>("a"_sd, operands.back()["a"]));
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(root->clone());
    }
}

void matchExpression_cloneOr(benchmark::State& state) {
    const size_t numPredicates = static_cast<size_t>(state.range());
    auto root = std::make_unique<OrMatchExpression>();
    std::vector<BSONObj> operands;
    operands.reserve(numPredicates);
    for (size_t predicateIndex = 0; predicateIndex < numPredicates; ++predicateIndex) {
        operands.emplace_back(BSON("a" << static_cast<int>(predicateIndex)));
        root->add(std::make_unique<EqualityMatchExpression>("a"_sd, operands.back()["a"]));
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(root->clone());
    }
}

void matchExpression_cloneAndOfOrs(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range());
    auto root = std::make_unique<AndMatchExpression>();
    std::vector<BSONObj> operands;
    operands.reserve(size);
    for (size_t index = 0; index < size; ++index) {
        auto orExpr = std::make_unique<OrMatchExpression>();
        operands.emplace_back(BSON("a" << static_cast<int>(index)));
        orExpr->add(std::make_unique<EqualityMatchExpression>("a"_sd, operands.back()["a"]));
        orExpr->add(std::make_unique<GTMatchExpression>("a"_sd, operands.back()["a"]));
        root->add(std::move(orExpr));
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(root->clone());
    }
}

BENCHMARK(matchExpression_createAnd)->RangeMultiplier(10)->Range(10, 10000);
BENCHMARK(matchExpression_createOr)->RangeMultiplier(10)->Range(10, 10000);
BENCHMARK(matchExpression_createAndOfOrs)->Args({3})->Args({7})->Args({10})->Args({13});

BENCHMARK(matchExpression_cloneAnd)->RangeMultiplier(10)->Range(10, 10000);
BENCHMARK(matchExpression_cloneOr)->RangeMultiplier(10)->Range(10, 10000);
BENCHMARK(matchExpression_cloneAndOfOrs)->Args({3})->Args({7})->Args({10})->Args({13});
}  // namespace mongo::boolean_simplification
