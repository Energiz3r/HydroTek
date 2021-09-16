
const config = require('./config')
const express = require('express')
const app = express();

const https = require("https"),
  fs = require("fs"),
  helmet = require("helmet");

const options = {
  key: fs.readFileSync(config.certKey),
  cert: fs.readFileSync(config.certCert)
};

app.use(helmet()); // Add Helmet as a middleware

//add the handler for the domain root /
app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

//set the directory for static content (eg.: bundle.js, images) 
app.use(express.static('./public'));

app.listen(8000);

https.createServer(options, app).listen(config.listenPort);

