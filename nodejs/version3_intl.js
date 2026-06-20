// http://localhost:3002/?n=10
// Usa el API nativa de JavaScript (Intl) sin librerías externas

const http = require('http');

const server = http.createServer((req, res) => {
  const url = new URL(req.url, `http://localhost:3002`);
  const n = url.searchParams.get('n');

  if (!n) {
    res.end('Proporciona un número: http://localhost:3002/?n=10');
    return;
  }

  const formatter = new Intl.NumberFormat('es', { style: 'decimal' });
  const resultado = new Intl.NumberFormat('es-MX', {
    style: 'currency',
    currency: 'MXN'
  });

  // Convertir número a letras en español nativo de JS
  const palabras = numberToSpanish(parseInt(n));
  res.end(palabras);
});

function numberToSpanish(n) {
  const unidades = ['','uno','dos','tres','cuatro','cinco','seis','siete','ocho','nueve',
    'diez','once','doce','trece','catorce','quince','dieciséis','diecisiete',
    'dieciocho','diecinueve','veinte'];
  const decenas = ['','','veinti','treinta','cuarenta','cincuenta','sesenta','setenta','ochenta','noventa'];
  const centenas = ['','ciento','doscientos','trescientos','cuatrocientos','quinientos',
    'seiscientos','setecientos','ochocientos','novecientos'];

  if (n === 0) return 'cero';
  if (n <= 20) return unidades[n];
  if (n < 30) return 'veinti' + unidades[n - 20];
  if (n < 100) return decenas[Math.floor(n/10)] + (n%10 ? ' y ' + unidades[n%10] : '');
  if (n === 100) return 'cien';
  if (n < 1000) return centenas[Math.floor(n/100)] + (n%100 ? ' ' + numberToSpanish(n%100) : '');
  if (n < 2000) return 'mil' + (n%1000 ? ' ' + numberToSpanish(n%1000) : '');
  if (n < 1000000) return numberToSpanish(Math.floor(n/1000)) + ' mil' + (n%1000 ? ' ' + numberToSpanish(n%1000) : '');
  return n.toString();
}

server.listen(3002, () => console.log('Servidor en http://localhost:3002/?n=10'));