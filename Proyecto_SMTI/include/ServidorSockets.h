#ifndef SERVIDOR_SOCKETS_H
#define SERVIDOR_SOCKETS_H

#include <string>
#include "Sensor.h"
#include "ListaGen.h"

// Trama ya interpretada (lo que llega por la red)
struct MuestraSensor {
    int idSensor;
    double valor;
    char tipo[12];
};

class ServidorSMTI {
private:
    int server_fd;
    int puerto;

    // Convierte una linea de texto en un sensor y lo mete en la lista.
    // Formato: TIPO;id;ubicacion;valor   Ej: TEMP;101;Caldera-01;92.5
    void procesarTrama(const std::string& trama, ListaGen<Sensor*>& lista);

public:
    ServidorSMTI(int _puerto);
    ~ServidorSMTI();

    // socket + bind + listen
    void iniciar();

    // accept + recv hasta que el cliente cierre la conexion
    void atenderCliente(ListaGen<Sensor*>& lista);
};

#endif
