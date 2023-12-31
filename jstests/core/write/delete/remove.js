// @tags: [requires_non_retryable_writes, requires_fastcount]

// remove.js
// unit test for db remove

let t = db.removetest;

function f(n, dir) {
    t.createIndex({x: dir || 1});
    for (let i = 0; i < n; i++)
        t.save({x: 3, z: "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"});

    assert.eq(n, t.find().count());
    t.remove({x: 3});

    assert.eq(0, t.find().count());

    assert(t.findOne() == null, "A:" + tojson(t.findOne()));
    assert(t.validate().valid, "B");
}

t.drop();
f(300, 1);

f(500, -1);

assert(t.validate().valid, "C");

// no query for remove() throws starting in 2.6
assert.throws(function() {
    db.t.remove();
});
