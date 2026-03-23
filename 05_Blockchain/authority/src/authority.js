const mqtt = require("mqtt");
const { MongoClient } = require("mongodb");
const CryptoJS = require("crypto-js");
const HmacSHA256 = require("crypto-js/hmac-sha256");

const { Data, Block, Blockchain } = require("../blockchain");

const MONGO_URL = process.env.MONGO_URL;
const CHAIN_DB = process.env.CHAIN_DB;

const MQTT_URL = process.env.MQTT_URL;
const MQTT_TOPICS = process.env.MQTT_TOPICS.split(",").map((s) => s.trim()).filter(Boolean);

const BLOCK_INTERVAL_MS = Number(process.env.BLOCK_INTERVAL_MS);
const MAX_TX_PER_BLOCK = Number(process.env.MAX_TX_PER_BLOCK);

const HMAC_KEY = process.env.HMAC_KEY;
const AUTHORITY_ID = process.env.AUTHORITY_ID;

let buffer = [];

function verifyHmac(tx) {
    const d = new Data({
        deviceId: tx.deviceId,
        timestamp: tx.timestamp,
        topic: tx.topic,
        data: tx.data,
        hmac: tx.hmac,
        publicKeyPem: tx.publicKeyPem,
        signatureB64: tx.signatureB64
    });

    const msg = d.signingMessage();
    const expected = HmacSHA256(msg, HMAC_KEY).toString(CryptoJS.enc.Hex);

    return expected === String(tx.hmac).toLowerCase();
}

async function main() {
    const client = new MongoClient(MONGO_URL);
    await client.connect();
    const db = client.db(CHAIN_DB);
    const blocks = db.collection("blocks");

    await blocks.createIndex({ height: 1 }, { unique: true });
    await blocks.createIndex({ hash: 1 }, { unique: true });

    const g = await blocks.findOne({ height: 0 });
    if (!g) {
        const genesis = Blockchain.createGenesis(AUTHORITY_ID);
        await blocks.insertOne({ ...genesis });
    }

    const m = mqtt.connect(MQTT_URL);
    m.on("connect", () => {
        MQTT_TOPICS.forEach((t) => m.subscribe(t));
    });

    m.on("message", (topic, buf) => {
        console.log(topic, buf.toString("utf8"));
        let obj = JSON.parse(buf.toString("utf8"));
        if (obj && obj.payload) obj = obj.payload; // si Node-RED envoie {topic,payload:{...}}

        const tx = {
            deviceId: obj.deviceId || obj.id,
            timestamp: obj.timestamp || Date.now(),
            topic: obj.topic || topic,
            data: obj.data,
            hmac: obj.hmac,
            publicKeyPem: obj.publicKeyPem || null,
            signatureB64: obj.signatureB64 || obj.signature || null
        };


        if (!verifyHmac(tx)) return;

        buffer.push(new Data(tx).toPlain());
    });

    setInterval(async () => {
        if (buffer.length === 0) return;

        const head = await blocks.find().sort({ height: -1 }).limit(1).next();
        const height = head.height + 1;

        const data = buffer.splice(0, MAX_TX_PER_BLOCK);

        const timestamp = Date.now();
        const previousHash = head.hash;

        const hash = Block.computeHash({ height, previousHash, timestamp, data, authorityId: AUTHORITY_ID });

        const block = {
            height,
            previousHash,
            timestamp,
            data,
            hash,
            authorityId: AUTHORITY_ID,
            authoritySigB64: null
        };

        await blocks.insertOne(block);
    }, BLOCK_INTERVAL_MS);
}

main();