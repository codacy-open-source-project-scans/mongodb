/**
 *    Copyright (C) 2018-present MongoDB, Inc.
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

#include "mongo/s/catalog_cache_loader.h"
#include "mongo/util/concurrency/thread_pool.h"

namespace mongo {

/**
 * Mocks the metadata refresh results with settable return values. The purpose of this class is to
 * facilitate testing of classes that use a CatalogCacheLoader.
 */
class CatalogCacheLoaderMock final : public CatalogCacheLoader {
    CatalogCacheLoaderMock(const CatalogCacheLoaderMock&) = delete;
    CatalogCacheLoaderMock& operator=(const CatalogCacheLoaderMock&) = delete;

public:
    CatalogCacheLoaderMock() = default;
    ~CatalogCacheLoaderMock() = default;

    /**
     * These functions should never be called. They trigger invariants if called.
     */
    void initializeReplicaSetRole(bool isPrimary) override;
    void onStepDown() override;
    void onStepUp() override;
    void shutDown() override;
    void notifyOfCollectionPlacementVersionUpdate(const NamespaceString& nss) override;
    void waitForCollectionFlush(OperationContext* opCtx, const NamespaceString& nss) override;
    void waitForDatabaseFlush(OperationContext* opCtx, StringData dbName) override;

    SemiFuture<CollectionAndChangedChunks> getChunksSince(const NamespaceString& nss,
                                                          ChunkVersion version) override;

    SemiFuture<DatabaseType> getDatabase(StringData dbName) override;

    /**
     * Sets the mocked collection entry result that getChunksSince will use to construct its return
     * value.
     */

    void setCollectionRefreshReturnValue(StatusWith<CollectionType> statusWithCollectionType);
    void clearCollectionReturnValue();

    /**
     * Sets the mocked chunk results that getChunksSince will use to construct its return value.
     */
    void setChunkRefreshReturnValue(StatusWith<std::vector<ChunkType>> statusWithChunks);
    void clearChunksReturnValue();

    /**
     * Sets the mocked database entry result that getDatabase will use to construct its return
     * value.
     */
    void setDatabaseRefreshReturnValue(StatusWith<DatabaseType> swDatabase);
    void clearDatabaseReturnValue();

    void setReshardingFields(boost::optional<TypeCollectionReshardingFields> reshardingFields) {
        _reshardingFields = std::move(reshardingFields);
    }

    void setCollectionRefreshValues(
        const NamespaceString& nss,
        StatusWith<CollectionType> statusWithCollection,
        StatusWith<std::vector<ChunkType>> statusWithChunks,
        boost::optional<TypeCollectionReshardingFields> reshardingFields) {
        _refreshValues[nss] = {statusWithCollection, statusWithChunks, reshardingFields};
    }

    static const Status kCollectionInternalErrorStatus;
    static const Status kChunksInternalErrorStatus;
    static const Status kDatabaseInternalErrorStatus;

private:
    StatusWith<CollectionType> _swCollectionReturnValue{kCollectionInternalErrorStatus};

    StatusWith<std::vector<ChunkType>> _swChunksReturnValue{kChunksInternalErrorStatus};

    boost::optional<TypeCollectionReshardingFields> _reshardingFields;

    struct RefreshInfo {
        StatusWith<CollectionType> swCollectionReturnValue{kCollectionInternalErrorStatus};
        StatusWith<std::vector<ChunkType>> swChunksReturnValue{kChunksInternalErrorStatus};
        boost::optional<TypeCollectionReshardingFields> reshardingFields;
    };

    stdx::unordered_map<NamespaceString, RefreshInfo> _refreshValues;
    StatusWith<DatabaseType> _swDatabaseReturnValue{kDatabaseInternalErrorStatus};
};

}  // namespace mongo
