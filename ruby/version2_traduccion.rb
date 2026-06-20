# http://localhost:4568/?n=10
# ruby version2_traduccion.rb

require 'sinatra'
require 'savon'
require 'httparty'
require 'uri'

set :port, 4568

get '/' do
  n = params['n']
  halt 400, 'Proporciona un número: http://localhost:4568/?n=10' if n.nil?

  # Llamada SOAP
  client = Savon.client(wsdl: 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL')
  response = client.call(:number_to_words, message: { ubi_num: n.to_i })
  en_ingles = response.body[:number_to_words_response][:number_to_words_result]

  # Traducción con MyMemory API
  url = "https://api.mymemory.translated.net/get?q=#{URI.encode_www_form_component(en_ingles)}&langpair=en|es"
  trad_response = HTTParty.get(url)
  traducido = trad_response['responseData']['translatedText']

  content_type 'text/plain; charset=utf-8'
  traducido
end