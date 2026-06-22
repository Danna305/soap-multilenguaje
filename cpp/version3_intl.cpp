// http://localhost:8102/?n=10
// cl version3_intl.cpp /link ws2_32.lib
// version3_intl.exe

#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::string convertirEspanol(long long n) {
    if (n == 0) return "cero";
    if (n < 0) return "menos " + convertirEspanol(-n);

    std::string unidades[] = {"","uno","dos","tres","cuatro","cinco","seis",
        "siete","ocho","nueve","diez","once","doce","trece","catorce","quince",
        "diecis\xc3\xa9is","diecisiete","dieciocho","diecinueve","veinte"};
    std::string decenas[] = {"","","veinte","treinta","cuarenta","cincuenta",
        "sesenta","setenta","ochenta","noventa"};
    std::string centenas[] = {"","ciento","doscientos","trescientos","cuatrocientos",
        "quinientos","seiscientos","setecientos","ochocientos","novecientos"};

    if (n <= 20) return unidades[n];
    if (n < 30) return "veinti" + unidades[n-20];
    if (n < 100) {
        std::string r = decenas[n/10];
        if (n%10) r += " y " + unidades[n%10];
        return r;
    }
    if (n == 100) return "cien";
    if (n < 1000) {
        std::string r = centenas[n/100];
        if (n%100) r += " " + convertirEspanol(n%100);
        return r;
    }
    if (n < 2000) {
        std::string r = "mil";
        if (n%1000) r += " " + convertirEspanol(n%1000);
        return r;
    }
    if (n < 1000000) {
        std::string r = convertirEspanol(n/1000) + " mil";
        if (n%1000) r += " " + convertirEspanol(n%1000);
        return r;
    }
    return std::to_string(n);
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

    std::string resultado = convertirEspanol(std::stoll(n));
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
    addr.sin_port = htons(8102);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(servidor, (sockaddr*)&addr, sizeof(addr));
    listen(servidor, 5);
    std::cout << "Servidor en http://localhost:8102/?n=10" << std::endl;
    while (true) {
        SOCKET cliente = accept(servidor, nullptr, nullptr);
        manejarCliente(cliente);
    }
    WSACleanup();
    return 0;
}