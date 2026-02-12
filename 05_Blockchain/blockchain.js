const crypto = require("crypto");

class Data {
    constructor({ deviceId, timestamp, payload, publicKeyPem, signatureB64 }) {
        this.deviceId = deviceId;
        this.timestamp = timestamp;
        this.payload = payload;
        this.publicKeyPem = publicKeyPem;     // PEM (optionnel si SKIP_SIG_CHECK=true)
        this.signatureB64 = signatureB64;     // base64 (optionnel si SKIP_SIG_CHECK=true)
    }

    signingMessage() {
        return `${this.deviceId}|${this.timestamp}|${JSON.stringify(this.payload)}`;
    }

    toPlain() {
        return {
            deviceId: this.deviceId,
            timestamp: this.timestamp,
            payload: this.payload,
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