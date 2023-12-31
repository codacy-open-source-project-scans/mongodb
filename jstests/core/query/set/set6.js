// Cannot implicitly shard accessed collections because of following errmsg: A single
// update/delete on a sharded collection must contain an exact match on _id or contain the shard
// key.
// @tags: [assumes_unsharded_collection]

let t = db.set6;
t.drop();

let x = {_id: 1, r: new DBRef("foo", new ObjectId())};
t.insert(x);
assert.eq(x, t.findOne(), "A");

x.r.$id = new ObjectId();
t.update({}, {$set: {r: x.r}});
assert.eq(x, t.findOne(), "B");

x.r2 = new DBRef("foo2", 5);
t.update({}, {$set: {"r2": x.r2}});
assert.eq(x, t.findOne(), "C");

x.r.$id = 2;
t.update({}, {$set: {"r.$id": 2}});
assert.eq(x.r.$id, t.findOne().r.$id, "D");
