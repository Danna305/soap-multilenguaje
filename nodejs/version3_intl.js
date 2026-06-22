// http://localhost:3002/?n=10

const http = require('http');

const server = http.createServer((req, res) => {
  const url = new URL(req.url, 'http://localhost:3002');
  const n = url.searchParams.get('n');

  if (!n) {
    res.end('Proporciona un número: http://localhost:3002/?n=10');
    return;
  }

  const palabras = numberToSpanish(parseInt(n));
  res.setHeader('Content-Type', 'text/plain; charset=utf-8');
  res.end(palabras);
});

function numberToSpanish(n) {
  const unidades = ['','uno','dos','tres','cuatro','cinco','seis','siete','ocho','nueve',
    'diez','once','doce','trece','catorce','quince','dieciséis','diecisiete',
    'dieciocho','diecinueve','veinte'];
  const decenas = ['','','veinte','treinta','cuarenta','cincuenta',
    'sesenta','setenta','ochenta','noventa'];
  const centenas = ['','ciento','doscientos','trescientos','cuatrocientos','quinientos',
    'seiscientos','setecientos','ochocientos','novecientos'];

  if (n === 0) return 'cero';
  if (n <= 20) return unidades[n];
  if (n < 30) return 'veinti' + unidades[n - 20];
  if (n < 100) {
    const dec = decenas[Math.floor(n/10)];
    return n%10 ? dec + ' y ' + unidades[n%10] : dec;
  }
  if (n === 100) return 'cien';
  if (n < 1000) {
    const cen = centenas[Math.floor(n/100)];
    return n%100 ? cen + ' ' + numberToSpanish(n%100) : cen;
  }
  if (n < 2000) return 'mil' + (n%1000 ? ' ' + numberToSpanish(n%1000) : '');
  if (n < 1000000) {
    const miles = numberToSpanish(Math.floor(n/1000)) + ' mil';
    return n%1000 ? miles + ' ' + numberToSpanish(n%1000) : miles;
  }
  return n.toString();
}

server.listen(3002, () => console.log('Servidor en http://localhost:3002/?n=10'));