# http://localhost:7071/?n=10
# perl version2_traduccion.pl

use strict;
use warnings;
use SOAP::Lite;
use LWP::UserAgent;
use HTTP::Daemon;
use HTTP::Status;
use URI;
use URI::Escape;
use JSON;

my $servidor = HTTP::Daemon->new(
    LocalAddr => 'localhost',
    LocalPort => 7071,
) or die "No se pudo iniciar el servidor: $!";

print "Servidor en http://localhost:7071/?n=10\n";

while (my $conexion = $servidor->accept()) {
    while (my $peticion = $conexion->get_request()) {
        my $uri = URI->new($peticion->uri);
        my %params = $uri->query_form;
        my $n = $params{n} // '0';

        # Llamada SOAP
        my $cliente = SOAP::Lite->service(
            'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL'
        );
        my $en_ingles = $cliente->NumberToWords($n);

        # Traducción con MyMemory API
        my $ua = LWP::UserAgent->new();
        my $url = "https://api.mymemory.translated.net/get?q=" .
                  uri_escape($en_ingles) . "&langpair=en|es";

        my $trad_resp = $ua->get($url);
        my $datos = decode_json($trad_resp->content);
        my $traducido = $datos->{responseData}{translatedText};

        my $respuesta = HTTP::Response->new(200);
        $respuesta->header('Content-Type' => 'text/plain; charset=utf-8');
        $respuesta->content($traducido);
        $conexion->send_response($respuesta);
    }
    $conexion->close();
}