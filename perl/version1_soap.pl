# http://localhost:7070/?n=10
# perl version1_soap.pl

use strict;
use warnings;
use SOAP::Lite;
use HTTP::Daemon;
use HTTP::Status;
use URI;

my $servidor = HTTP::Daemon->new(
    LocalAddr => 'localhost',
    LocalPort => 7070,
) or die "No se pudo iniciar el servidor: $!";

print "Servidor en http://localhost:7070/?n=10\n";

while (my $conexion = $servidor->accept()) {
    while (my $peticion = $conexion->get_request()) {
        my $uri = URI->new($peticion->uri);
        my %params = $uri->query_form;
        my $n = $params{n} // '0';

        my $cliente = SOAP::Lite->service(
            'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL'
        );

        my $resultado = $cliente->NumberToWords($n);

        my $respuesta = HTTP::Response->new(200);
        $respuesta->header('Content-Type' => 'text/plain; charset=utf-8');
        $respuesta->content($resultado);
        $conexion->send_response($respuesta);
    }
    $conexion->close();
}