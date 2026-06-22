// http://localhost:3001/?n=10

const http = require('http');

const SOAP_URL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso';

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, 'http://localhost:3001');
  const n = url.searchParams.get('n');

  if (!n) {
    res.end('Proporciona un número: http://localhost:3001/?n=10');
    return;
  }

  try {
    const soap = `<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>${n}</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>`;

    // Llamada SOAP
    const soapResp = await fetch(SOAP_URL, {
      method: 'POST',
      headers: { 
        'Content-Type': 'text/xml; charset=utf-8',
        'SOAPAction': '""'
      },
      body: soap
    });

    const soapText = await soapResp.text();
    console.log('Respuesta SOAP:', soapText.substring(0, 300));

    // Parsear respuesta con o sin namespace
    let enIngles = '';
    if (soapText.includes('NumberToWordsResult>')) {
      enIngles = soapText
        .split('NumberToWordsResult>')[1]
        .split('<')[0]
        .trim();
    } else {
      res.end('Error: no se encontró resultado en: ' + soapText.substring(0, 200));
      return;
    }

    console.log('En inglés:', enIngles);

    // Traducción
    const tradResp = await fetch(
      `https://api.mymemory.translated.net/get?q=${encodeURIComponent(enIngles)}&langpair=en|es`
    );
    const tradJson = await tradResp.json();
    const traducido = tradJson.responseData.translatedText;

    res.setHeader('Content-Type', 'text/plain; charset=utf-8');
    res.end(traducido);

  } catch (err) {
    console.log('Error completo:', err);
    res.end('Error: ' + err.message);
  }
});

server.listen(3001, () => console.log('Servidor en http://localhost:3001/?n=10'));