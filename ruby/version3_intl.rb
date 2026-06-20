# http://localhost:4569/?n=10
# ruby version3_intl.rb

require 'sinatra'

set :port, 4569

get '/' do
  n = params['n']
  halt 400, 'Proporciona un número: http://localhost:4569/?n=10' if n.nil?

  content_type 'text/plain; charset=utf-8'
  convertir_espanol(n.to_i)
end

def convertir_espanol(n)
  return 'cero' if n == 0
  return 'menos ' + convertir_espanol(-n) if n < 0

  unidades = ['', 'uno', 'dos', 'tres', 'cuatro', 'cinco',
              'seis', 'siete', 'ocho', 'nueve', 'diez', 'once', 'doce',
              'trece', 'catorce', 'quince', 'dieciséis', 'diecisiete',
              'dieciocho', 'diecinueve', 'veinte']
  decenas = ['', '', 'veinte', 'treinta', 'cuarenta',
             'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa']
  centenas = ['', 'ciento', 'doscientos', 'trescientos',
              'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos',
              'ochocientos', 'novecientos']

  return unidades[n] if n <= 20
  return 'veinti' + unidades[n - 20] if n < 30

  if n < 100
    res = decenas[n / 10]
    res += ' y ' + unidades[n % 10] if n % 10 != 0
    return res
  end

  return 'cien' if n == 100

  if n < 1000
    res = centenas[n / 100]
    res += ' ' + convertir_espanol(n % 100) if n % 100 != 0
    return res
  end

  if n < 2000
    res = 'mil'
    res += ' ' + convertir_espanol(n % 1000) if n % 1000 != 0
    return res
  end

  if n < 1_000_000
    res = convertir_espanol(n / 1000) + ' mil'
    res += ' ' + convertir_espanol(n % 1000) if n % 1000 != 0
    return res
  end

  n.to_s
end