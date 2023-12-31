// @tags: [requires_multi_updates, requires_non_retryable_writes]

let t = db.update_multi3;

function test(useIndex) {
    t.drop();

    if (useIndex)
        t.createIndex({k: 1});

    for (let i = 0; i < 10; i++) {
        t.save({_id: i, k: 'x', a: []});
    }

    t.update({k: 'x'}, {$push: {a: 'y'}}, false, true);

    t.find({k: "x"}).forEach(function(z) {
        assert.eq(["y"], z.a, "useIndex: " + useIndex);
    });
}

test(false);
test(true);
