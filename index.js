const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const port = process.env.PORT || 3000;
const bodyParser = require('body-parser');

app.use(bodyParser.urlencoded({ limit: '200mb', extended: true }));
app.use(bodyParser.json({ limit: '200mb', type: 'application/json' }));

app.use(express.static(__dirname + '/public'));

server.listen(port, () => console.log(`Example app listening on port ${port}!`))
