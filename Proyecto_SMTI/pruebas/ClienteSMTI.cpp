// Cliente de prueba: manda varias tramas al servidor (una de ellas con error)
#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define CERRAR_SOCKET closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #define CERRAR_SOCKET close
#endif

int main() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "No se pudo inicializar Winsock" << std::endl;
        return 1;
    }
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "No se pudo crear el socket" << std::endl;
        return 1;
    }

    sockaddr_in serv;
    std::memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        std::cerr << "No se pudo conectar (el servidor esta corriendo?)" << std::endl;
        CERRAR_SOCKET(sock);
        return 1;
    }

    std::string tramas =
        "TEMP;101;Caldera-01;92.5\n"
        "TEMP;102;Horno-02;65.0\n"
        "PRES;201;Tanque-A;135.2\n"
        "CAUD;301;Tuberia-3;4.8\n"
        "XXXX;999;Nada;1.0\n"      // tipo invalido
        "TEMP;101;Repetido;70.0\n" // id repetido
        "TEMP;abc;Mala;10\n";      // id no numerico

    send(sock, tramas.c_str(), tramas.length(), 0);
    std::cout << "Tramas enviadas." << std::endl;

    CERRAR_SOCKET(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
