db.server5782.drop();
db.server5782.save({string: "foo"});

// check that without $literal we end up comparing a field with itself and the result is true
var result = db.runCommand({
    aggregate: "server5782",
    pipeline: [{$project: {stringis$string: {$eq: ["$string", '$string']}}}],
    cursor: {}
});
assert.eq(result.cursor.firstBatch[0].stringis$string, true);

// check that with $literal we end up comparing a field with '$string' and the result is true
result = db.runCommand({
    aggregate: "server5782",
    pipeline: [{$project: {stringis$string: {$eq: ["$string", {$literal: '$string'}]}}}],
    cursor: {}
});
assert.eq(result.cursor.firstBatch[0].stringis$string, false);
