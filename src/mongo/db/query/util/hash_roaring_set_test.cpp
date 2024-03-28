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

#include "mongo/db/query/util/hash_roaring_set.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
struct OnSwitchToRoaring {

    OnSwitchToRoaring(bool& isSwitched) : _isSwitched(isSwitched) {}

    void operator()() {
        _isSwitched = true;
    }

    bool& _isSwitched;
};

/**
 * Tests Hash Table part of HashRoaringSet.
 */
TEST(HashRoaringTest, HashSet) {
    bool isSwitched = false;
    HashRoaringSet set(10'000, 10, 1'000'000, OnSwitchToRoaring{isSwitched});

    for (uint64_t i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            ASSERT_TRUE(set.addChecked(i));
        }
    }

    // Make sure that the set is not switched to Roaring Bitmaps.
    ASSERT_FALSE(isSwitched);

    for (uint64_t i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            ASSERT_FALSE(set.addChecked(i));
        } else {
            ASSERT_TRUE(set.addChecked(i));
        }
    }

    // Make sure that the set is not switched to Roaring Bitmaps.
    ASSERT_FALSE(isSwitched);
}

/**
 * Tests Roaring Bitmaps part of HashRoaringSet.
 */
TEST(HashRoaringTest, RoaringBitmaps) {
    bool isSwitched = false;
    HashRoaringSet hashRoaring(1'000, 10, 1'000'000, OnSwitchToRoaring{isSwitched});

    for (uint64_t i = 0; i < 10000; ++i) {
        if (i % 3 == 0) {
            ASSERT_TRUE(hashRoaring.addChecked(i));
        }
    }

    // Make sure that the set is switched to Roaring Bitmaps.
    ASSERT_TRUE(isSwitched);

    for (uint64_t i = 0; i < 10000; ++i) {
        if (i % 3 == 0) {
            ASSERT_FALSE(hashRoaring.addChecked(i));
        } else {
            ASSERT_TRUE(hashRoaring.addChecked(i));
        }
    }

    for (uint64_t i = 0; i < 10000; ++i) {
        ASSERT_FALSE(hashRoaring.addChecked(i));
    }
}

/**
 * Tests combined Hash Table and Roaring Bitmaps parts of HashRoaringSet.
 */
TEST(HashRoaringTest, HashTableAndRoaringBitmaps) {
    bool isSwitched = false;
    HashRoaringSet hashRoaring(1'000, 1, 1'000'000, OnSwitchToRoaring{isSwitched});

    for (uint64_t i = 0; i < 3333; ++i) {
        if (i % 3 == 0) {
            ASSERT_TRUE(hashRoaring.addChecked(i));
        }
    }

    // The migration is ongoing.
    ASSERT_EQ(hashRoaring.getCurrentState(), HashRoaringSet::kHashTableAndBitmap);

    for (uint64_t i = 1000; i >= 300; --i) {
        if (i % 3 == 0) {
            ASSERT_FALSE(hashRoaring.addChecked(i));
        } else {
            ASSERT_TRUE(hashRoaring.addChecked(i));
        }
    }

    // The migration is still ongoing.
    ASSERT_EQ(hashRoaring.getCurrentState(), HashRoaringSet::kHashTableAndBitmap);
}

/**
 * Test the migration of elements from the internal hash table to Roaring Bitmaps.
 */
TEST(HashRoaringTest, Migration) {
    bool isSwitched = false;
    HashRoaringSet hashRoaring(1'000, 10, 1'000'000, OnSwitchToRoaring{isSwitched});

    ASSERT_TRUE(hashRoaring.addChecked(777777));
    ASSERT_FALSE(hashRoaring.addChecked(777777));

    // The migration hasn't started yet.
    ASSERT_EQ(hashRoaring.getCurrentState(), HashRoaringSet::kHashTable);

    for (uint64_t i = 0; i < 3100; ++i) {
        if (i % 3 == 0) {
            hashRoaring.addChecked(i);
        }
    }

    // Make sure that the set is switched to Roaring Bitmaps.
    ASSERT_TRUE(isSwitched);

    // The migration is ongoing.
    ASSERT_EQ(hashRoaring.getCurrentState(), HashRoaringSet::kHashTableAndBitmap);

    for (uint64_t i = 0; i < 10000; ++i) {
        hashRoaring.addChecked(i);
    }

    // The migration is completed.
    ASSERT_EQ(hashRoaring.getCurrentState(), HashRoaringSet::kBitmap);
}
}  // namespace mongo
