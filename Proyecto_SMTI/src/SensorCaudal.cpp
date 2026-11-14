#include "SensorCaudal.h"

SensorCaudal::SensorCaudal(int _id, std::string _ub, double _lect, double _min)
    : Sensor(_id, _ub, _lect), caudalMinimo(_min) {}

void SensorCaudal::procesarLectura() {
    std::cout << "[CAUDAL] " << *this << " L/min";
    // aqui la alerta es al reves: si el caudal baja demasiado
    if (lectura < caudalMinimo) {
        std::cout << " --> ALERTA: CAUDAL MUY BAJO!";
    }
    std::cout << std::endl;
}
