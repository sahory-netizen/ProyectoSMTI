#include "ServidorSockets.h"
#include "ExcepcionesSMTI.h"
#include "SensorTemperatura.h"
#include "SensorPresion.h"
#include "SensorCaudal.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

// Los sockets de Linux (POSIX) y los de Windows (Winsock) son casi iguales.
// Estas lineas escogen las cabeceras segun el sistema donde se compile.
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int LongDir;
    #define CERRAR_SOCKET closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    typedef socklen_t LongDir;
    #define CERRAR_SOCKET close
#endif

ServidorSMTI::ServidorSMTI(int _puerto) : server_fd(-1), puerto(_puerto) {}

ServidorSMTI::~ServidorSMTI() {
    if (server_fd >= 0) {
        CERRAR_SOCKET(server_fd);
    }
#ifdef _WIN32
    WSACleanup();
#endif
}

void ServidorSMTI::iniciar() {
#ifdef _WIN32
    // Winsock hay que encenderlo antes de usar cualquier socket
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        throw ErrorRed("Error Winsock: no se pudo inicializar WSAStartup.");
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw ErrorRed("Error POSIX: No se pudo crear el socket.");
    }

    // para poder reiniciar el servidor sin esperar a que se libere el puerto
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(puerto);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw ErrorRed("Error POSIX: bind fallido, puerto " + std::to_string(puerto) +
                       " ocupado o sin permisos.");
    }

    if (listen(server_fd, 5) < 0) {
        throw ErrorRed("Error POSIX: listen fallido.");
    }

    std::cout << "[SERVIDOR] Escuchando telemetria en puerto " << puerto << "..." << std::endl;
}

void ServidorSMTI::atenderCliente(ListaGen<Sensor*>& lista) {
    sockaddr_in cliente_addr;
    LongDir len = sizeof(cliente_addr);

    // accept se queda esperando hasta que llegue un cliente
    int client_fd = accept(server_fd, (struct sockaddr*)&cliente_addr, &len);
    if (client_fd < 0) {
        throw ErrorRed("Error POSIX: no se pudo aceptar al cliente.");
    }
    std::cout << "[SERVIDOR] Cliente conectado." << std::endl;

    std::string pendiente;   // texto recibido que aun no forma una linea completa
    char buffer[1024];

    while (true) {
        int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            CERRAR_SOCKET(client_fd);
            throw ErrorRed("Error POSIX: fallo al leer con recv.");
        }
        if (n == 0) {
            break;   // el cliente cerro la conexion
        }

        buffer[n] = '\0';
        pendiente += buffer;

        // sacar del texto cada linea completa (terminada en \n)
        size_t pos;
        while ((pos = pendiente.find('\n')) != std::string::npos) {
            std::string linea = pendiente.substr(0, pos);
            pendiente.erase(0, pos + 1);

            // por si el cliente manda \r\n (Windows)
            if (!linea.empty() && linea[linea.size() - 1] == '\r') {
                linea.erase(linea.size() - 1);
            }
            if (linea.empty()) continue;

            // una trama mala no debe tumbar el servidor
            try {
                procesarTrama(linea, lista);
            } catch (const ErrorTrama& e) {
                std::cout << "[TRAMA RECHAZADA] " << e.what() << std::endl;
            }
        }
    }

    CERRAR_SOCKET(client_fd);
    std::cout << "[SERVIDOR] Cliente desconectado. Sensores guardados: "
              << lista.getTam() << std::endl;
}

void ServidorSMTI::procesarTrama(const std::string& trama, ListaGen<Sensor*>& lista) {
    // partir la trama por ';'
    std::vector<std::string> campos;
    std::stringstream ss(trama);
    std::string campo;
    while (std::getline(ss, campo, ';')) {
        campos.push_back(campo);
    }

    if (campos.size() != 4) {
        throw ErrorTrama("formato invalido (se esperan 4 campos): " + trama);
    }

    MuestraSensor m;
    std::strncpy(m.tipo, campos[0].c_str(), sizeof(m.tipo) - 1);
    m.tipo[sizeof(m.tipo) - 1] = '\0';

    try {
        m.idSensor = std::stoi(campos[1]);
        m.valor = std::stod(campos[3]);
    } catch (const std::exception&) {
        throw ErrorTrama("id o valor no numerico: " + trama);
    }

    std::string ubicacion = campos[2];

    if (lista.buscarPorId(m.idSensor) != nullptr) {
        throw ErrorTrama("el sensor " + std::to_string(m.idSensor) + " ya existe");
    }

    // se crea en el Heap el tipo de sensor que corresponda
    Sensor* nuevo = nullptr;
    if (std::strcmp(m.tipo, "TEMP") == 0) {
        nuevo = new SensorTemperatura(m.idSensor, ubicacion, m.valor);
    } else if (std::strcmp(m.tipo, "PRES") == 0) {
        nuevo = new SensorPresion(m.idSensor, ubicacion, m.valor);
    } else if (std::strcmp(m.tipo, "CAUD") == 0) {
        nuevo = new SensorCaudal(m.idSensor, ubicacion, m.valor);
    } else {
        throw ErrorTrama(std::string("tipo de sensor desconocido: ") + m.tipo);
    }

    lista.insertar(nuevo);
    std::cout << "[SERVIDOR] Sensor " << m.idSensor << " (" << m.tipo << ") guardado." << std::endl;
}
