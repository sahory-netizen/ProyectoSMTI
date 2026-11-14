#include <iostream>
#include <stdexcept>
#include "Sensor.h"
#include "ListaGen.h"
#include "ServidorSockets.h"

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << " SMTI: SERVIDOR DE TELEMETRIA DISTRIBUIDA (UNIDAD 9)" << std::endl;
    std::cout << "==================================================" << std::endl;

    ListaGen<Sensor*> listaPlanta;

    try {
        ServidorSMTI servidor(8080);
        servidor.iniciar();
        servidor.atenderCliente(listaPlanta);

        std::cout << "\n--- PROCESAMIENTO POLIMORFICO DE TELEMETRIA ---" << std::endl;
        listaPlanta.procesarTodo();

    } catch (const std::exception& e) {
        std::cerr << "[EXCEPCION GRAVE SMTI] " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--- Fin del programa, se libera la lista ---" << std::endl;
    return 0;   // aqui se llama ~ListaGen() y se borran los sensores
}
