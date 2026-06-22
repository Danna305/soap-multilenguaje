// Developer Command Prompt:
// cl version1_soap.cpp /I C:\vcpkg\installed\x86-windows\include /link C:\vcpkg\installed\x86-windows\lib\libcurl.lib ws2_32.lib
// version1_soap.exe
// http://localhost:8100/?n=10

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

std::string llamarSOAP(const std::string& n) {
    CURL* curl = curl_easy_init();
    std::string respuesta;

    if (curl) {
        std::string soap = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
            "<soap:Body>"
            "<NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">"
            "<ubiNum>" + n + "</ubiNum>"
            "</NumberToWords>"
            "</soap:Body>"
            "</soap:Envelope>";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");

        curl_easy_setopt(curl, CURLOPT_URL, "https://www.dataaccess.com/webservicesserver/NumberConversion.wso");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soap.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

     size_t inicio = respuesta.find("NumberToWordsResult>") + 20;
     size_t fin = respuesta.find("<", inicio);
     return respuesta.substr(inicio, fin - inicio);
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

    std::string resultado = llamarSOAP(n);
    std::string respuesta = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n" + resultado;
    send(cliente, respuesta.c_str(), respuesta.size(), 0);
    closesocket(cliente);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET servidor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8100);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(servidor, (sockaddr*)&addr, sizeof(addr));
    listen(servidor, 5);
    std::cout << "Servidor en http://localhost:8100/?n=10" << std::endl;

    while (true) {
        SOCKET cliente = accept(servidor, nullptr, nullptr);
        manejarCliente(cliente);
    }

    WSACleanup();
    return 0;
}