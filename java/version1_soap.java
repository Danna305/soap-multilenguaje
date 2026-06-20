// Compilar: javac -cp . version1_soap.java
// Ejecutar: java -cp . version1_soap 10
// O como servidor web en puerto 8080: http://localhost:8080/?n=10

import com.sun.net.httpserver.HttpServer;
import java.io.*;
import java.net.*;
import java.net.http.*;

public class version1_soap {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/", exchange -> {
            String query = exchange.getRequestURI().getQuery();
            String n = "0";
            if (query != null && query.contains("n=")) {
                n = query.split("n=")[1];
            }

            String soap = """
                <?xml version="1.0" encoding="utf-8"?>
                <soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
                  <soap:Body>
                    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
                      <ubiNum>""" + n + """
                    </ubiNum>
                    </NumberToWords>
                  </soap:Body>
                </soap:Envelope>
                """;

            HttpClient client = HttpClient.newHttpClient();
            HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create("https://www.dataaccess.com/webservicesserver/NumberConversion.wso"))
                .header("Content-Type", "text/xml; charset=utf-8")
                .POST(HttpRequest.BodyPublishers.ofString(soap))
                .build();

            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
            String body = response.body();
            String result = body.split("<NumberToWordsResult>")[1].split("</NumberToWordsResult>")[0];

            byte[] resp = result.getBytes();
            exchange.sendResponseHeaders(200, resp.length);
            exchange.getResponseBody().write(resp);
            exchange.getResponseBody().close();
        });

        server.start();
        System.out.println("Servidor en http://localhost:8080/?n=10");
    }
}