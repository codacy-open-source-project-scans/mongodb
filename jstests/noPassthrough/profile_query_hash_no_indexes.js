// Confirms that profile entries for find commands contain the appropriate query hash.
// Same as profile_query_hash.js, but runs on a collection with no indexes to test the Bonsai plan
// cache on M2-eligible queries.
//
// @tags: [
//   # The test runs commands that are not allowed with security token: setProfilingLevel.
//   not_allowed_with_signed_security_token,
//   assumes_against_mongod_not_mongos,
//   assumes_read_concern_unchanged,
//   does_not_support_stepdowns,
//   requires_profiling,
// ]

import {getLatestProfilerEntry} from "jstests/libs/profiler.js";

// Prevent the mongo shell from gossiping its cluster time, since this will increase the amount
// of data logged for each op. For some of the testcases below, including the cluster time would
// cause them to be truncated at the 512-byte RamLog limit, and some of the fields we need to
// check would be lost.
TestData.skipGossipingClusterTime = true;

const conn = MongoRunner.runMongod(
    {setParameter: {"failpoint.enableExplainInBonsai": tojson({mode: "alwaysOn"})}});
assert.neq(null, conn, "mongod was unable to start up");
const testDB = conn.getDB("jstests_query_shape_hash");
const coll = testDB.test;

// Utility function to list query shapes in cache. The length of the list of query shapes
// returned is used to validate the number of query hashes accumulated.
assert.commandWorked(coll.insert({a: 1, b: 1}));
assert.commandWorked(coll.insert({a: 1, b: 2}));
assert.commandWorked(coll.insert({a: 1, b: 2}));
assert.commandWorked(coll.insert({a: 2, b: 2}));

// Don't profile the setFCV command, which could be run during this test in the
// fcv_upgrade_downgrade_replica_sets_jscore_passthrough suite.
assert.commandWorked(testDB.setProfilingLevel(
    1, {filter: {'command.setFeatureCompatibilityVersion': {'$exists': false}}}));

// Executes query0 and gets the corresponding system.profile entry.
assert.eq(1,
          coll.find({a: 1, b: 1}, {a: 1}).comment("Query0 find command").itcount(),
          'unexpected document count');
const profileObj0 =
    getLatestProfilerEntry(testDB, {op: "query", "command.comment": "Query0 find command"});
assert(profileObj0.hasOwnProperty("planCacheKey"), tojson(profileObj0));

// Executes query1 and gets the corresponding system.profile entry.
assert.eq(0,
          coll.find({a: 2, b: 1}, {a: 1}).comment("Query1 find command").itcount(),
          'unexpected document count');
const profileObj1 =
    getLatestProfilerEntry(testDB, {op: "query", "command.comment": "Query1 find command"});
assert(profileObj1.hasOwnProperty("planCacheKey"), tojson(profileObj1));

// Check that the query shapes are the same.
assert.eq(
    profileObj0.planCacheKey, profileObj1.planCacheKey, 'unexpected not matching query hashes');

// Test that the planCacheKey is the same in explain output for query0 and query1 as it was
// in system.profile output.
const explainQuery0 = assert.commandWorked(
    coll.find({a: 1, b: 1}, {a: 1}).comment("Query0 find command").explain("queryPlanner"));
assert.eq(explainQuery0.queryPlanner.planCacheKey,
          profileObj0.planCacheKey,
          {explainQuery0, profileObj0});
const explainQuery1 = assert.commandWorked(
    coll.find({a: 2, b: 1}, {a: 1}).comment("Query1 find command").explain("queryPlanner"));
assert.eq(explainQuery1.queryPlanner.planCacheKey, profileObj0.planCacheKey, explainQuery1);

// Check that the 'planCacheKey' is the same for both query 0 and query 1.
assert.eq(explainQuery0.queryPlanner.planCacheKey, explainQuery1.queryPlanner.planCacheKey);

// Executes query2 and gets the corresponding system.profile entry.
assert.eq(0,
          coll.find({a: 12000, b: 1}).comment("Query2 find command").itcount(),
          'unexpected document count');
const profileObj2 =
    getLatestProfilerEntry(testDB, {op: "query", "command.comment": "Query2 find command"});
assert(profileObj2.hasOwnProperty("planCacheKey"), tojson(profileObj2));

// Query0 and query1 should both have the same query hash. Whereas, query2
// should have a unique hash.
assert.neq(profileObj0.planCacheKey, profileObj2.planCacheKey, 'unexpected matching query hashes');

// The planCacheKey in explain should be different for query2 than the hash from query0 and
// query1.
const explainQuery2 = assert.commandWorked(
    coll.find({a: 12000, b: 1}).comment("Query2 find command").explain("queryPlanner"));
assert(explainQuery2.queryPlanner.hasOwnProperty("planCacheKey"));
assert.neq(explainQuery2.queryPlanner.planCacheKey, profileObj0.planCacheKey, explainQuery2);
assert.eq(explainQuery2.queryPlanner.planCacheKey, profileObj2.planCacheKey, explainQuery2);

MongoRunner.stopMongod(conn);
