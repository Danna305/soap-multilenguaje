# http://localhost:4567/?n=10
# ruby version1_soap.rb

require 'sinatra'
require 'savon'

set :port, 4567

get '/' do
  n = params['n']
  halt 400, 'Proporciona un número: http://localhost:4567/?n=10' if n.nil?

  client = Savon.client(wsdl: 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL')
  response = client.call(:number_to_words, message: { ubi_num: n.to_i })
  result = response.body[:number_to_words_response][:number_to_words_result]

  content_type 'text/plain; charset=utf-8'
  result
end