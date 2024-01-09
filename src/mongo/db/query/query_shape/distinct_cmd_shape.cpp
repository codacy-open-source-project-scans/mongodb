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

#include "mongo/db/query/query_shape/distinct_cmd_shape.h"

#include "mongo/bson/simple_bsonobj_comparator.h"
#include "mongo/db/query/query_shape/shape_helpers.h"

namespace mongo::query_shape {

DistinctCmdShapeComponents::DistinctCmdShapeComponents(
    const ParsedDistinctCommand& request, const boost::intrusive_ptr<ExpressionContext>& expCtx)
    : key(request.distinctCommandRequest->getKey().toString()),
      representativeQuery(request.query->serialize(
          SerializationOptions::kRepresentativeQueryShapeSerializeOptions)) {}

void DistinctCmdShapeComponents::HashValue(absl::HashState state) const {
    absl::HashState::combine(std::move(state), key, simpleHash(representativeQuery));
}

int64_t DistinctCmdShapeComponents::size() const {
    return sizeof(this) + key.size() + representativeQuery.objsize();
}

const CmdSpecificShapeComponents& DistinctCmdShape::specificComponents() const {
    return components;
}

DistinctCmdShape::DistinctCmdShape(const ParsedDistinctCommand& distinct,
                                   const boost::intrusive_ptr<ExpressionContext>& expCtx)
    : Shape(distinct.distinctCommandRequest->getNamespaceOrUUID(),
            distinct.distinctCommandRequest->getCollation().get_value_or(BSONObj())),
      components(distinct, expCtx) {}

void DistinctCmdShape::appendCmdSpecificShapeComponents(BSONObjBuilder& bob,
                                                        OperationContext* opCtx,
                                                        const SerializationOptions& opts) const {
    // Command name.
    bob.append("command", DistinctCommandRequest::kCommandName);

    // Key.
    bob.append(DistinctCommandRequest::kKeyFieldName,
               opts.serializeFieldPathFromString(components.key));

    // Query.
    if (!components.representativeQuery.isEmpty()) {
        if (opts == SerializationOptions::kRepresentativeQueryShapeSerializeOptions) {
            // Fastpath. Already serialized using the same serialization options.
            bob.append(DistinctCommandRequest::kQueryFieldName, components.representativeQuery);
        } else {
            auto expCtx = ExpressionContext::makeBlankExpressionContext(opCtx, nssOrUUID);
            auto matchExpr = uassertStatusOK(
                MatchExpressionParser::parse(components.representativeQuery,
                                             expCtx,
                                             ExtensionsCallbackNoop(),
                                             MatchExpressionParser::kAllowAllSpecialFeatures));
            bob.append(DistinctCommandRequest::kQueryFieldName, matchExpr->serialize(opts));
        }
    }
}

QueryShapeHash DistinctCmdShape::sha256Hash(OperationContext*, const SerializationContext&) const {
    return SHA256Block::computeHash({
        ConstDataRange(DistinctCommandRequest::kCommandName.data(),
                       DistinctCommandRequest::kCommandName.size()),
        nssOrUUID.asDataRange(),
        components.representativeQuery.asDataRange(),
        ConstDataRange(components.key.data(), components.key.size()),
        collation.asDataRange(),
    });
}

}  // namespace mongo::query_shape
