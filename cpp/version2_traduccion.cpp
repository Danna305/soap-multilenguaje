// http://localhost:8101/?n=10
// cl version2_traduccion.cpp /I C:\vcpkg\installed\x86-windows\include /link C:\vcpkg\installed\x86-windows\lib\libcurl.lib ws2_32.lib
// version2_traduccion.exe

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string httpPost(const std::string& url, const std::string& datos, const std::string& contentType) {
    CURL* curl = curl_easy_init();
    std::string respuesta;
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Content-Type: " + contentType).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, datos.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return respuesta;
}

std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string respuesta;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return respuesta;
}

void manejarCliente(SOCKET cliente) {
    char buffer[4096] = {0};
    recv(cliente, buffer, 4096, 0);
    std::string peticion(buffer);

    std::string n = "0";
    size_t pos = peticion.find("n=");
    if (pos != std::string::npos) {
        size_t fin = peticion.find(" ", pos);
        n = peticion.substr(pos + 2, fin - pos - 2);
    }

    // SOAP
    std::string soap = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
        "<soap:Body><NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">"
        "<ubiNum>" + n + "</ubiNum></NumberToWords></soap:Body></soap:Envelope>";

    std::string soapResp = httpPost(
        "https://www.dataaccess.com/webservicesserver/NumberConversion.wso",
        soap, "text/xml; charset=utf-8"
    );

    size_t ini = soapResp.find("<NumberToWordsResult>") + 21;
    size_t fin2 = soapResp.find("</NumberToWordsResult>");
    std::string enIngles = soapResp.substr(ini, fin2 - ini);

    // Traducción
    std::string urlTrad = "https://api.mymemory.translated.net/get?q=" + enIngles + "&langpair=en|es";
    std::string tradResp = httpGet(urlTrad);

    size_t tIni = tradResp.find("\"translatedText\":\"") + 18;
    size_t tFin = tradResp.find("\"", tIni);
    std::string traducido = tradResp.substr(tIni, tFin - tIni);

    std::string respuesta = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n" + traducido;
    send(cliente, respuesta.c_str(), respuesta.size(), 0);
    closesocket(cliente);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET servidor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8101);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(servidor, (sockaddr*)&addr, sizeof(addr));
    listen(servidor, 5);
    std::cout << "Servidor en http://localhost:8101/?n=10" << std::endl;
    while (true) {
        SOCKET cliente = accept(servidor, nullptr, nullptr);
        manejarCliente(cliente);
    }
    WSACleanup();
    return 0;
}