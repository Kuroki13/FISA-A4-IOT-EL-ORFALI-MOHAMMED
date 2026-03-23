const crypto = require("crypto");

class Data {
    constructor({ deviceId, timestamp, topic, data, hmac, publicKeyPem, signatureB64 }) {
        this.deviceId = deviceId;
        this.timestamp = timestamp;
        this.topic = topic;
        this.data = data;
        this.hmac = hmac;
        this.publicKeyPem = publicKeyPem;
        this.signatureB64 = signatureB64;
    }

    signingMessage() {
        return `id=${this.deviceId}&data=${this.data}`;
    }

    toPlain() {
        return {
            deviceId: this.deviceId,
            timestamp: this.timestamp,
            topic: this.topic,
            data: this.data,
            hmac: this.hmac,
            publicKeyPem: this.publicKeyPem,
            signatureB64: this.signatureB64,
        };
    }
}

class Block {
    constructor({ height, previousHash, timestamp, data, hash, authorityId, authoritySigB64 }) {
        this.height = height;
        this.previousHash = previousHash;
        this.timestamp = timestamp;
        this.data = data;
        this.hash = hash;
        this.authorityId = authorityId;
        this.authoritySigB64 = authoritySigB64;
    }

    static computeHash({ height, previousHash, timestamp, data, authorityId }) {
        const payload = `${height}|${previousHash}|${timestamp}|${authorityId}|${JSON.stringify(data)}`;
        return crypto.createHash("sha256").update(payload).digest("hex");
    }
}

class Blockchain {
    static createGenesis(authorityId) {
        const height = 0;
        const previousHash = "0";
        const timestamp = Date.now();
        const data = [];
        const hash = Block.computeHash({ height, previousHash, timestamp, data, authorityId });
        return new Block({ height, previousHash, timestamp, data, hash, authorityId, authoritySigB64: null });
    }
}

module.exports = { Data, Block, Blockchain };