require('dotenv').config();
const fetch = require('node-fetch');
const HTMLparser = require('node-html-parser');
const Pool = require('pg').Pool

const pool = new Pool({
  user: process.env.DB_USER,
  host: process.env.DB_HOST,
  database: process.env.DB_NAME,
  password: process.env.DB_PASS,
  port: process.env.DB_PORT,
})

function fetchHTML () {
	const url = 'https://cinkciarz.pl/wymiana-walut/kursy-walut-cinkciarz-pl/eur/pln/';
	return fetch(url)
		.then(res => res.text())
		.then(HTMLparser.parse)
}

function prepareData (html) {
	const ratesDiv = html.querySelector('#currencies-rates').childNodes[3].childNodes[1].childNodes[1].childNodes[3].childNodes[1];
	const buyingRateRawText = ratesDiv.childNodes[3].structuredText;
	const sellingRateRawText = ratesDiv.childNodes[5].structuredText;
	return {
		buyingRate: parseFloat(buyingRateRawText.replace(',', '.')),
		sellingRate: parseFloat(sellingRateRawText.replace(',', '.')),
		timestamp: Date.now(),
	}
}

function insertData ({buyingRate, sellingRate, timestamp}) {
	return pool.query('INSERT INTO eur_pln (buying_rate, selling_rate, timestamp) VALUES ($1, $2, $3)', [buyingRate, sellingRate, timestamp], (error, results) => {
    if (error) {
      throw error
    }
  });
}

function writeExchangeRate () {
	return fetchHTML()
	.then(prepareData)
	.then(insertData)
	.then(() => console.log(`Inserted 1 row to eur_pln table at ${Date()}`))
	.catch(console.log);
}

writeExchangeRate();
module.exports = writeExchangeRate;
