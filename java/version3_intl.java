// http://localhost:8082/?n=10
// javac -cp . version3_intl.java
// java -cp . version3_intl

import com.sun.net.httpserver.HttpServer;
import java.io.*;
import java.net.*;

public class version3_intl {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8082), 0);
        server.createContext("/", exchange -> {
            try {
                String query = exchange.getRequestURI().getQuery();
                String n = "0";
                if (query != null && query.contains("n=")) {
                    n = query.split("n=")[1];
                }

                long numero = Long.parseLong(n);
                String resultado = convertirAEspanol(numero);

                byte[] resp = resultado.getBytes("UTF-8");
                exchange.getResponseHeaders().set("Content-Type", "text/plain; charset=UTF-8");
                exchange.sendResponseHeaders(200, resp.length);
                exchange.getResponseBody().write(resp);
                exchange.getResponseBody().close();

            } catch (Exception e) {
                byte[] resp = ("Error: " + e.getMessage()).getBytes();
                exchange.sendResponseHeaders(500, resp.length);
                exchange.getResponseBody().write(resp);
                exchange.getResponseBody().close();
            }
        });

        server.start();
        System.out.println("Servidor en http://localhost:8082/?n=10");
    }

    static String convertirAEspanol(long n) {
        if (n == 0) return "cero";
        if (n < 0) return "menos " + convertirAEspanol(-n);

        String[] unidades = {"", "uno", "dos", "tres", "cuatro", "cinco",
            "seis", "siete", "ocho", "nueve", "diez", "once", "doce",
            "trece", "catorce", "quince", "dieciséis", "diecisiete",
            "dieciocho", "diecinueve", "veinte"};
        String[] decenas = {"", "", "veinte", "treinta", "cuarenta",
            "cincuenta", "sesenta", "setenta", "ochenta", "noventa"};
        String[] centenas = {"", "ciento", "doscientos", "trescientos",
            "cuatrocientos", "quinientos", "seiscientos", "setecientos",
            "ochocientos", "novecientos"};

        if (n <= 20) return unidades[(int) n];
        if (n < 30) return "veinti" + unidades[(int)(n - 20)];
        if (n < 100) return decenas[(int)(n/10)] + (n%10 != 0 ? " y " + unidades[(int)(n%10)] : "");
        if (n == 100) return "cien";
        if (n < 1000) return centenas[(int)(n/100)] + (n%100 != 0 ? " " + convertirAEspanol(n%100) : "");
        if (n < 2000) return "mil" + (n%1000 != 0 ? " " + convertirAEspanol(n%1000) : "");
        if (n < 1000000) return convertirAEspanol(n/1000) + " mil" + (n%1000 != 0 ? " " + convertirAEspanol(n%1000) : "");
        return String.valueOf(n);
    }
}