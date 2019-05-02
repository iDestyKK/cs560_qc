var redis = require("redis");

let client = redis.createClient();

client.flushall();

client.set("test", "hello").bind(client);
