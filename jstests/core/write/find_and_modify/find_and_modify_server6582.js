// Cannot implicitly shard accessed collections because of following errmsg: A single
// update/delete on a sharded collection must contain an exact match on _id or contain the shard
// key.
// @tags: [assumes_unsharded_collection]

let t = db.find_and_modify_server6582;

t.drop();
let x =
    t.runCommand("findAndModify", {query: {f: 1}, update: {$set: {f: 2}}, upsert: true, new: true});
let le = x.lastErrorObject;
assert.eq(le.updatedExisting, false);
assert.eq(le.n, 1);
assert.eq(le.upserted, x.value._id);

t.drop();
t.insert({f: 1});
x = t.runCommand("findAndModify", {query: {f: 1}, remove: true});
le = x.lastErrorObject;
assert.eq(le.n, 1);
