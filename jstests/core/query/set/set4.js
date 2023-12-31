// Cannot implicitly shard accessed collections because of following errmsg: A single
// update/delete on a sharded collection must contain an exact match on _id or contain the shard
// key.
// @tags: [assumes_unsharded_collection]

let t = db.set4;
t.drop();

let orig = {_id: 1, a: [{x: 1}]};
t.insert(orig);

t.update({}, {$set: {'a.0.x': 2, 'foo.bar': 3}});
orig.a[0].x = 2;
orig.foo = {
    bar: 3
};
assert.eq(orig, t.findOne(), "A");

t.update({}, {$set: {'a.0.x': 4, 'foo.bar': 5}});
orig.a[0].x = 4;
orig.foo.bar = 5;
assert.eq(orig, t.findOne(), "B");
