/**
 *    Copyright (C) 2020-present MongoDB, Inc.
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

#include "mongo/db/repl/tenant_migration_conflict_info.h"
#include "mongo/base/init.h"  // IWYU pragma: keep
#include "mongo/base/string_data.h"

namespace mongo {

namespace {

MONGO_INIT_REGISTER_ERROR_EXTRA_INFO(TenantMigrationConflictInfo);
MONGO_INIT_REGISTER_ERROR_EXTRA_INFO(NonRetryableTenantMigrationConflictInfo);

constexpr StringData kMigrationIdFieldName = "migrationId"_sd;

}  // namespace

void TenantMigrationConflictInfoBase::serialize(BSONObjBuilder* bob) const {
    _migrationId.appendToBuilder(bob, kMigrationIdFieldName);
}

std::shared_ptr<const ErrorExtraInfo> TenantMigrationConflictInfoBase::parse(const BSONObj& obj) {
    auto uuid = uassertStatusOK(UUID::parse(obj[kMigrationIdFieldName]));
    return std::make_shared<TenantMigrationConflictInfoBase>(std::move(uuid));
}

}  // namespace mongo
