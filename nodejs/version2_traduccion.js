// http://localhost:3001/?n=10
// Traduce el resultado de inglés a español con Google Translate

const http = require('http');
const soap = require('soap');
const { translate } = require('@vitalets/google-translate-api');

const WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://localhost:3001`);
  const n = url.searchParams.get('n');

  if (!n) {
    res.end('Proporciona un número: http://localhost:3001/?n=10');
    return;
  }

  const client = await soap.createClientAsync(WSDL);
  const result = await client.NumberToWordsAsync({ ubiNum: parseInt(n) });
  const enIngles = result[0].NumberToWordsResult;

  const traduccion = await translate(enIngles, { to: 'es' });
  res.end(traduccion.text);
});

server.listen(3001, () => console.log('Servidor en http://localhost:3001/?n=10'));