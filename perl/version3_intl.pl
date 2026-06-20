# http://localhost:7072/?n=10
# perl version3_intl.pl

use strict;
use warnings;
use HTTP::Daemon;
use HTTP::Status;
use URI;

my $servidor = HTTP::Daemon->new(
    LocalAddr => 'localhost',
    LocalPort => 7072,
) or die "No se pudo iniciar el servidor: $!";

print "Servidor en http://localhost:7072/?n=10\n";

while (my $conexion = $servidor->accept()) {
    while (my $peticion = $conexion->get_request()) {
        my $uri = URI->new($peticion->uri);
        my %params = $uri->query_form;
        my $n = $params{n} // '0';

        my $resultado = convertir_espanol(int($n));

        my $respuesta = HTTP::Response->new(200);
        $respuesta->header('Content-Type' => 'text/plain; charset=utf-8');
        $respuesta->content($resultado);
        $conexion->send_response($respuesta);
    }
    $conexion->close();
}

sub convertir_espanol {
    my ($n) = @_;
    return 'cero' if $n == 0;
    return 'menos ' . convertir_espanol(-$n) if $n < 0;

    my @unidades = ('', 'uno', 'dos', 'tres', 'cuatro', 'cinco',
                    'seis', 'siete', 'ocho', 'nueve', 'diez', 'once', 'doce',
                    'trece', 'catorce', 'quince', "diecis\x{e9}is", 'diecisiete',
                    'dieciocho', 'diecinueve', 'veinte');
    my @decenas = ('', '', 'veinte', 'treinta', 'cuarenta',
                   'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa');
    my @centenas = ('', 'ciento', 'doscientos', 'trescientos',
                    'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos',
                    'ochocientos', 'novecientos');

    return $unidades[$n] if $n <= 20;
    return 'veinti' . $unidades[$n - 20] if $n < 30;

    if ($n < 100) {
        my $res = $decenas[int($n / 10)];
        $res .= ' y ' . $unidades[$n % 10] if $n % 10 != 0;
        return $res;
    }

    return 'cien' if $n == 100;

    if ($n < 1000) {
        my $res = $centenas[int($n / 100)];
        $res .= ' ' . convertir_espanol($n % 100) if $n % 100 != 0;
        return $res;
    }

    if ($n < 2000) {
        my $res = 'mil';
        $res .= ' ' . convertir_espanol($n % 1000) if $n % 1000 != 0;
        return $res;
    }

    if ($n < 1_000_000) {
        my $res = convertir_espanol(int($n / 1000)) . ' mil';
        $res .= ' ' . convertir_espanol($n % 1000) if $n % 1000 != 0;
        return $res;
    }

    return "$n";
}