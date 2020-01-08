const express = require('express');
const dotenv = require('dotenv');
dotenv.config();
yaml = require('js-yaml');
fs = require('fs');

// Constants
const PORT = process.env.PORT || 8080;
const HOST = process.env.HOST || '0.0.0.0';

try {
	const translations = yaml.safeLoad(fs.readFileSync('translations.yaml', 'utf8'));
	const lang = process.env.LANG || 'en';
	const text = translations[lang] || 'Hello world';
	const app = express();
	app.get('/', (_, res) => {
		res.send(`${text}\n`);
	});

	app.listen(PORT, HOST);
	console.log(`Running on http://${HOST}:${PORT}`);
} catch (e) {
  console.log(e);
}
