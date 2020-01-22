## Setup
please run
```sh
yarn
```
to install required packages.

PostgreSQL is used in this project, therefore also setup database, create user and run:
```sh
CREATE TABLE eur_pln (id SERIAL PRIMARY KEY, buying_rate REAL, selling_rate REAL, timestamp INT);
```
Beside that, create `.env` from `.env_example` and fill it with values.

## Description
This is a simple node.js app, which contains of three parts:
- an app, which downloads exchange rates from eur to pln from website 
`https://cinkciarz.pl/wymiana-walut/kursy-walut-cinkciarz-pl/eur/pln/` and writes them to database (PostgreSQL is used). Both buying and selling rates are written. In order to run it type
	```sh
	yarn write
	```
- scheduler, which runs 'write' above every minute
In order to run it type
	```sh
	yarn schedule
	```
- api with two endpoints:
	-	{{HOST}}/mean, which calculates mean
	- {{HOST}}/variance, which calculates variance
	Both endpoints require a query parameter 'column', which can be 'buying_rate' or 'selling_rate'. There are also two optional parameters as 'startTime' and 'endTime' which accept unix timestamp in milliseconds. In order to run it type
	```sh
	yarn api
	```
