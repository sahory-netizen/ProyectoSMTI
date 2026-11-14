#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <string>

// Clase base abstracta: no se pueden crear objetos Sensor directamente
class Sensor {
protected:
    int id;
    std::string ubicacion;
    double lectura;

public:
    Sensor(int _id, std::string _ub, double _lect);

    // virtual para que al hacer delete con un Sensor* se llame
    // tambien al destructor de la clase hija
    virtual ~Sensor();

    int getId() const;
    double getLectura() const;

    // metodo virtual puro: cada sensor lo implementa a su manera
    virtual void procesarLectura() = 0;

    friend std::ostream& operator<<(std::ostream& os, const Sensor& s);
};

#endif
