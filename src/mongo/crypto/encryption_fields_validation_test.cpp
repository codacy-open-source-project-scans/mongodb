/**
 *    Copyright (C) 2022-present MongoDB, Inc.
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

#include <string>

#include "mongo/base/string_data.h"
#include "mongo/crypto/encryption_fields_validation.h"
#include "mongo/platform/decimal128.h"
#include "mongo/unittest/assert.h"
#include "mongo/unittest/framework.h"

namespace mongo {


TEST(FLEValidationUtils, ValidateDoublePrecisionRange) {
    ASSERT(validateDoublePrecisionRange(3.000, 0));
    ASSERT(validateDoublePrecisionRange(3.000, 1));
    ASSERT(validateDoublePrecisionRange(3.000, 2));
    ASSERT(validateDoublePrecisionRange(-3.000, 0));
    ASSERT(validateDoublePrecisionRange(-3.000, 1));
    ASSERT(validateDoublePrecisionRange(-3.000, 2));

    ASSERT_FALSE(validateDoublePrecisionRange(3.100, 0));
    ASSERT(validateDoublePrecisionRange(3.100, 1));
    ASSERT(validateDoublePrecisionRange(3.100, 2));
    ASSERT_FALSE(validateDoublePrecisionRange(-3.100, 0));
    ASSERT(validateDoublePrecisionRange(-3.100, 1));
    ASSERT(validateDoublePrecisionRange(-3.100, 2));

    ASSERT(validateDoublePrecisionRange(1.000, 3));
    ASSERT(validateDoublePrecisionRange(-1.000, 3));

    ASSERT_FALSE(validateDoublePrecisionRange(3.140, 1));
    ASSERT_FALSE(validateDoublePrecisionRange(-3.140, 1));

    ASSERT(validateDoublePrecisionRange(0.000, 0));
    ASSERT(validateDoublePrecisionRange(0.000, 1));
    ASSERT(validateDoublePrecisionRange(0.000, 50));
    ASSERT(validateDoublePrecisionRange(-0.000, 0));
    ASSERT(validateDoublePrecisionRange(-0.000, 1));
    ASSERT(validateDoublePrecisionRange(-0.000, 50));
}

bool validateDecimal128PrecisionRangeTest(std::string s, uint32_t precision) {
    Decimal128 dec(s);
    return validateDecimal128PrecisionRange(dec, precision);
}

TEST(FLEValidationUtils, ValidateDecimalPrecisionRange) {

    ASSERT(validateDecimal128PrecisionRangeTest("3.000", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("3.000", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("3.000", 2));
    ASSERT(validateDecimal128PrecisionRangeTest("-3.000", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("-3.000", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("-3.000", 2));

    ASSERT_FALSE(validateDecimal128PrecisionRangeTest("3.100", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("3.100", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("3.100", 2));
    ASSERT_FALSE(validateDecimal128PrecisionRangeTest("-3.100", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("-3.100", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("-3.100", 2));

    ASSERT_FALSE(validateDecimal128PrecisionRangeTest("3.140", 1));
    ASSERT_FALSE(validateDecimal128PrecisionRangeTest("-3.140", 1));

    ASSERT(validateDecimal128PrecisionRangeTest("0.000", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("0.000", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("0.000", 50));
    ASSERT(validateDecimal128PrecisionRangeTest("-0.000", 0));
    ASSERT(validateDecimal128PrecisionRangeTest("-0.000", 1));
    ASSERT(validateDecimal128PrecisionRangeTest("-0.000", 50));
}

}  // namespace mongo
