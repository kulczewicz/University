require('dotenv').config();
const express = require('express');
const app = express();
const Pool = require('pg').Pool;

const pool = new Pool({
  user: process.env.DB_USER,
  host: process.env.DB_HOST,
  database: process.env.DB_NAME,
  password: process.env.DB_PASS,
  port: process.env.DB_PORT,
})

const rates = {
	BUYING_RATE: 'buying_rate',
	SELLING_RATE: 'selling_rate',
}

function calcMean (array) {
	const sum = array.reduce((a, b) => a + b, 0)
	return sum / array.length;
};

function calcVariance (array) {
	const mean = calcMean(array);
	return calcMean(array.map(num => Math.pow(num - mean, 2)));
};

function prepareQueryAndAgrs (startTime, endTime, column) {
	let query = `SELECT ${column} FROM eur_pln`;
	let args = [];
	if (startTime) {
		query += ' WHERE timestamp >= $1';
		args.push(startTime);
		if (endTime) {
			query += ' AND timestamp <= $2';
			args.push(endTime);
		}
	} else if (endTime) {
		query += ' WHERE timestamp <= $1';
		args.push(endTime);
	}
	query += ';';
	return { query, args };
}

function getValuesByColumn (req) {
	const startTime = parseInt(req.query.startTime);
	const endTime = parseInt(req.query.endTime);
	const column = req.query.column;
	console.log(startTime, endTime, column);
	if (!Object.values(rates).includes(column)) {
		return Promise.reject({status: 400, message: 'Column does not exist'});
	}
	const { query, args } = prepareQueryAndAgrs (startTime, endTime, column);
	return new Promise((resolve, reject) => {
		pool.query(query, args, (error, results) => {
			if (error) {
				return reject({status: 500, message: error})
			}
			return resolve(results.rows.map(obj => obj[column]));
		});
	});
}

function getMean (req, res) {
	return getValuesByColumn(req)
		.then(calcMean)
		.then(mean => res.status(200).json({ mean }))
		.catch(error => res.status(error.status).json({ error: error.message }))
}

function getVariance (req, res) {
	return getValuesByColumn(req)
		.then(calcVariance)
		.then(variance => res.status(200).json({ variance }))
		.catch(error => res.status(error.status).json({ error: error.message }))
}

app.get('/mean', getMean);
app.get('/variance', getVariance);

app.listen(process.env.APP_PORT, () => {
	console.log(`App running on port ${process.env.APP_PORT}`)
})
