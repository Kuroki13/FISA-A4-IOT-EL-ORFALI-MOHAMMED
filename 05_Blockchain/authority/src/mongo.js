const { MongoClient } = require("mongodb");

async function ensureIndex(col, key, options) {
  const wantedName = options?.name;
  const indexes = await col.listIndexes().toArray();

  if (wantedName && indexes.some((i) => i.name === wantedName)) {
    return;
  }

  const keyJson = JSON.stringify(key);
  const already = indexes.some((i) => JSON.stringify(i.key) === keyJson);
  if (already) return;

  await col.createIndex(key, options);
}

async function connectMongo({ url, dbName }) {
  const client = new MongoClient(url);
  await client.connect();
  const db = client.db(dbName);

  const blocks = db.collection("blocks");
  const pending = db.collection("pending");
  const meta = db.collection("meta");

  await ensureIndex(blocks, { hash: 1 }, { unique: true, name: "blocks_hash_uq" });
  await ensureIndex(blocks, { height: 1 }, { unique: true, name: "blocks_height_uq" });

  await ensureIndex(pending, { status: 1, timestamp: 1 }, { name: "pending_status_ts" });

  return { client, blocks, pending, meta };
}

module.exports = { connectMongo };