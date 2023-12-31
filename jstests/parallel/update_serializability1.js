
let t = db.update_serializability1;
t.drop();

let N = 100000;

let bulk = t.initializeUnorderedBulkOp();
for (var i = 0; i < N; i++) {
    bulk.insert({_id: i, a: i, b: N - i, x: 1, y: 1});
}
bulk.execute();

t.createIndex({a: 1});
t.createIndex({b: 1});

let s1 = startParallelShell(
    "db.update_serializability1.update( { a : { $gte : 0 } }, { $set : { b : " + (N + 1) +
    ", x : 2 } }, false, true );");
let s2 = startParallelShell("db.update_serializability1.update( { b : { $lte : " + N +
                            " } }, { $set : { a : -1, y : 2 } }, false, true );");

s1();
s2();

// some of each type should have gotten done
assert(t.find({x: 2}).count() > 0);
assert(t.find({y: 2}).count() > 0);

// both operations should never happen on a document
assert.eq(0, t.find({x: 2, y: 2}).count());
