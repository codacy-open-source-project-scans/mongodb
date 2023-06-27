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

#pragma once

#include "mongo/db/s/move_primary/move_primary_state_machine_gen.h"
#include <type_traits>

namespace mongo {

namespace move_primary_metrics {

template <class T>
inline constexpr bool isStateDocument =
    std::disjunction_v<std::is_same<T, MovePrimaryRecipientDocument>,
                       std::is_same<T, MovePrimaryDonorDocument>>;

template <typename T>
inline constexpr auto getState(const T& document) {
    static_assert(isStateDocument<T>);
    if constexpr (std::is_same_v<T, MovePrimaryDonorDocument>) {
        return document.getMutableFields().getState();
    } else {
        return document.getState();
    }
}

template <typename T>
inline constexpr ShardingDataTransformMetrics::Role getRoleForStateDocument() {
    static_assert(isStateDocument<T>);
    using Role = ShardingDataTransformMetrics::Role;
    if constexpr (std::is_same_v<T, MovePrimaryDonorDocument>) {
        return Role::kDonor;
    } else {
        return Role::kRecipient;
    }
}

}  // namespace move_primary_metrics

}  // namespace mongo
