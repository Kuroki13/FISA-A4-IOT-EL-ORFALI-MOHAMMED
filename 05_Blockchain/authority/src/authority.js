const fs = require("fs");
const crypto = require("crypto");
const { connectMongo } = require("./mongo");
const { Blockchain, Block, Data } = require("../blockchain");

const MONGO_URL = process.env.MONGO_URL;
const CHAIN_DB = process.env.CHAIN_DB;
const AUTHORITY_ID = process.env.AUTHORITY_ID || "poa-1";
const BLOCK_INTERVAL_MS = Number(process.env.BLOCK_INTERVAL_MS || 60000);
const MAX_TX_PER_BLOCK = Number(process.env.MAX_TX_PER_BLOCK || 200);
const SKIP_SIG_CHECK = (process.env.SKIP_SIG_CHECK || "true").toLowerCase() === "true";

const privPath = process.env.AUTHORITY_PRIV_PATH;
const pubPath = process.env.AUTHORITY_PUB_PATH;

const privateKey = privPath && fs.existsSync(privPath) ? crypto.createPrivateKey(fs.readFileSync(privPath, "utf8")) : null;
const publicKeyPem = pubPath && fs.existsSync(pubPath) ? fs.readFileSync(pubPath, "utf8") : null;

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

function verifyArduinoSignature(tx) {
    if (SKIP_SIG_CHECK) return true;
    if (!tx.publicKeyPem || !tx.signatureB64) return false;

    const d = new Data(tx);
    const msg = Buffer.from(d.signingMessage(), "utf8");
    const sig = Buffer.from(tx.signatureB64, "base64");

    try {
        const pub = crypto.createPublicKey(tx.publicKeyPem);
        return crypto.verify(null, msg, pub, sig);
    } catch {
        return false;
    }
}

function signBlockHash(hashHex) {
    if (!privateKey) return null;
    return crypto.sign(null, Buffer.from(hashHex, "hex"), privateKey).toString("base64");
}

async function ensureGenesis(blocks, meta) {
    const g = await blocks.findOne({ height: 0 });
    if (g) return g;

    const genesis = Blockchain.createGenesis(AUTHORITY_ID);
    const sig = signBlockHash(genesis.hash);

    await blocks.insertOne({
        ...genesis,
        authoritySigB64: sig,
        authorityPubPem: publicKeyPem,
        createdAt: new Date(),
    });

    await meta.updateOne({ _id: "chain" }, { $set: { bestHeight: 0, updatedAt: new Date() } }, { upsert: true });
    return blocks.findOne({ height: 0 });
}

(async function main() {
    const { client, blocks, pending, meta } = await connectMongo({ url: MONGO_URL, dbName: CHAIN_DB });
    console.log(`[AUTHORITY ${AUTHORITY_ID}] up (skipSigCheck=${SKIP_SIG_CHECK})`);

    process.on("SIGINT", async () => {
        await client.close();
        process.exit(0);
    });

    await ensureGenesis(blocks, meta);

    while (true) {
        const head = await blocks.find().sort({ height: -1 }).limit(1).next();

        const txs = await pending
            .find({ status: "pending" })
            .sort({ timestamp: 1 })
            .limit(MAX_TX_PER_BLOCK)
            .toArray();

        if (!txs.length) {
            await sleep(500);
            continue;
        }

        // Vérif signatures Arduino (si activé)
        const valid = [];
        const invalidIds = [];
        for (const t of txs) {
            if (verifyArduinoSignature(t)) valid.push(t);
            else invalidIds.push(t._id);
        }

        if (invalidIds.length) {
            await pending.updateMany(
                { _id: { $in: invalidIds } },
                { $set: { status: "rejected", rejectedAt: new Date(), rejectReason: "Bad signature" } }
            );
        }

        if (!valid.length) {
            await sleep(300);
            continue;
        }

        const height = head.height + 1;
        const previousHash = head.hash;
        const timestamp = Date.now();
        const data = valid.map((t) => new Data(t).toPlain());

        const hash = Block.computeHash({ height, previousHash, timestamp, data, authorityId: AUTHORITY_ID });
        const authoritySigB64 = signBlockHash(hash);

        await blocks.insertOne({
            height,
            previousHash,
            timestamp,
            data,
            hash,
            authorityId: AUTHORITY_ID,
            authoritySigB64,
            authorityPubPem: publicKeyPem,
            createdAt: new Date(),
        });

        await pending.updateMany(
            { _id: { $in: valid.map((t) => t._id) } },
            { $set: { status: "included", includedAt: new Date(), includedInHeight: height } }
        );

        await meta.updateOne({ _id: "chain" }, { $set: { bestHeight: height, updatedAt: new Date() } }, { upsert: true });

        console.log(`[AUTHORITY] block height=${height} tx=${valid.length} hash=${hash.slice(0, 10)}...`);

        await sleep(BLOCK_INTERVAL_MS);
    }
})().catch((e) => {
    console.error("AUTHORITY fatal:", e);
    process.exit(1);
});