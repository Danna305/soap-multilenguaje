// http://localhost:8080/?n=10
// javac -cp . version1_soap.java
// java -cp . version1_soap

import com.sun.net.httpserver.HttpServer;
import java.io.*;
import java.net.*;
import java.net.http.*;

public class version1_soap {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/", exchange -> {
            try {
                String query = exchange.getRequestURI().getQuery();
                String n = "0";
                if (query != null && query.contains("n=")) {
                    n = query.split("n=")[1];
                }

                String soap = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                    + "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
                    + "<soap:Body>"
                    + "<NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">"
                    + "<ubiNum>" + n + "</ubiNum>"
                    + "</NumberToWords>"
                    + "</soap:Body>"
                    + "</soap:Envelope>";

                HttpClient client = HttpClient.newHttpClient();
                HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create("https://www.dataaccess.com/webservicesserver/NumberConversion.wso"))
                    .header("Content-Type", "text/xml; charset=utf-8")
                    .POST(HttpRequest.BodyPublishers.ofString(soap))
                    .build();

                HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
                String body = response.body();

                String result;
                if (body.contains("NumberToWordsResult>")) {
                    result = body.split("NumberToWordsResult>")[1].split("<")[0];
                } else {
                    result = "Error: " + body.substring(0, 200);
                }

                byte[] resp = result.getBytes("UTF-8");
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
        System.out.println("Servidor en http://localhost:8080/?n=10");
    }
}