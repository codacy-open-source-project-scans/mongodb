// Cannot implicitly shard accessed collections because of following errmsg: A single
// update/delete on a sharded collection must contain an exact match on _id or contain the shard
// key.
// @tags: [assumes_unsharded_collection]

let t = db.update_arraymatch4;
t.drop();

let x = {_id: 1, arr: ["A1", "B1", "C1"]};
t.insert(x);
assert.eq(x, t.findOne(), "A1");

x.arr[0] = "A2";
t.update({arr: "A1"}, {$set: {"arr.$": "A2"}});
assert.eq(x, t.findOne(), "A2");

t.createIndex({arr: 1});
x.arr[0] = "A3";
t.update({arr: "A2"}, {$set: {"arr.$": "A3"}});
assert.eq(x, t.findOne(), "A3");  // SERVER-1055
