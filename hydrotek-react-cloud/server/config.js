//
//  configuration 
//

const listenPort = 80
const maxHttpBufferSize = 1800000; //1800kb
const sessionExpiry = 30; //minutes
const certKey = "C:\\Certbot\\live\\home.noobs.wtf\\privkey.pem"
const certCert = "C:\\Certbot\\live\\home.noobs.wtf\\fullchain.pem"

module.exports = {
  maxHttpBufferSize,
  sessionExpiry,
  listenPort,
  certKey,
  certCert
};
