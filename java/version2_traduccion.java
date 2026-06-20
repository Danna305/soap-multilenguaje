// http://localhost:8081/?n=10
// Traduce usando MyMemory API (gratuita, sin instalar nada)

import com.sun.net.httpserver.HttpServer;
import java.io.*;
import java.net.*;
import java.net.http.*;

public class version2_traduccion {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8081), 0);
        server.createContext("/", exchange -> {
            String query = exchange.getRequestURI().getQuery();
            String n = "0";
            if (query != null && query.contains("n=")) {
                n = query.split("n=")[1];
            }

            // Llamada SOAP
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
            HttpRequest soapReq = HttpRequest.newBuilder()
                .uri(URI.create("https://www.dataaccess.com/webservicesserver/NumberConversion.wso"))
                .header("Content-Type", "text/xml; charset=utf-8")
                .POST(HttpRequest.BodyPublishers.ofString(soap))
                .build();

            HttpResponse<String> soapResp = client.send(soapReq, HttpResponse.BodyHandlers.ofString());
            String enIngles = soapResp.body()
                .split("<NumberToWordsResult>")[1]
                .split("</NumberToWordsResult>")[0].trim();

            // Traducción con MyMemory API
            String urlTraduccion = "https://api.mymemory.translated.net/get?q="
                + URLEncoder.encode(enIngles, "UTF-8") + "&langpair=en|es";

            HttpRequest tradReq = HttpRequest.newBuilder()
                .uri(URI.create(urlTraduccion))
                .GET()
                .build();

            HttpResponse<String> tradResp = client.send(tradReq, HttpResponse.BodyHandlers.ofString());
            String tradBody = tradResp.body();
            String traducido = tradBody.split("\"translatedText\":\"")[1].split("\"")[0];

            byte[] resp = traducido.getBytes("UTF-8");
            exchange.getResponseHeaders().set("Content-Type", "text/plain; charset=UTF-8");
            exchange.sendResponseHeaders(200, resp.length);
            exchange.getResponseBody().write(resp);
            exchange.getResponseBody().close();
        });

        server.start();
        System.out.println("Servidor en http://localhost:8081/?n=10");
    }
}