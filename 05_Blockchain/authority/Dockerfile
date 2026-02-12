FROM node:20-alpine

WORKDIR /app

COPY authority/package.json ./package.json
RUN npm install --omit=dev

COPY blockchain.js ./blockchain.js
COPY authority/src ./src

CMD ["node", "src/authority.js"]