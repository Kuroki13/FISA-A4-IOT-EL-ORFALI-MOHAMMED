const mqtt = require("mqtt");
const SHA256 = require("crypto-js/sha256");
const { ec: EC } = require("elliptic");

const ec = new EC("secp256k1");
const key = ec.genKeyPair();

const deviceId = "SIM-01";
const timestamp = Date.now();
const payload = { temp: 22.5, hum: 40.1 };

const message = `${deviceId}|${timestamp}|temp=${payload.temp}|hum=${payload.hum}`;
const hash = SHA256(message).toString();
const sig = key.sign(hash);

const tx = {
  deviceId,
  timestamp,
  payload,
  message,
  publicKeyHex: key.getPublic("hex"),
  signature: { r: sig.r.toString("hex"), s: sig.s.toString("hex"), recoveryParam: sig.recoveryParam }
};

const client = mqtt.connect("mqtt://localhost:1883");
client.on("connect", () => {
  client.publish("sensors/data", JSON.stringify(tx));
  client.end();
});