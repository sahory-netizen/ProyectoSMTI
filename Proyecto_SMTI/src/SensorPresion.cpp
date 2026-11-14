#include "SensorPresion.h"

SensorPresion::SensorPresion(int _id, std::string _ub, double _lect, double _max)
    : Sensor(_id, _ub, _lect), presionMaxima(_max) {}

void SensorPresion::procesarLectura() {
    std::cout << "[PRESION] " << *this << " psi";
    if (lectura > presionMaxima) {
        std::cout << " --> ALERTA: PRESION POR ENCIMA DEL LIMITE!";
    }
    std::cout << std::endl;
}
