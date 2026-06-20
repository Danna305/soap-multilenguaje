// Uso: node version1_soap.js 10
// O con servidor: node version1_soap.js server
// http://localhost:3000/?n=10

const http = require('http');
const soap = require('soap');

const WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://localhost:3000`);
  const n = url.searchParams.get('n');

  if (!n) {
    res.end('Proporciona un número: http://localhost:3000/?n=10');
    return;
  }

  const client = await soap.createClientAsync(WSDL);
  const result = await client.NumberToWordsAsync({ ubiNum: parseInt(n) });
  res.end(result[0].NumberToWordsResult);
});

server.listen(3000, () => console.log('Servidor en http://localhost:3000/?n=10'));