const cron = require('node-cron');
const writeExchangeRate = require('./write');

cron.schedule("* * * * *", () => {
  writeExchangeRate();
});