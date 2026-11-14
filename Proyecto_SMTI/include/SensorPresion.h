#ifndef SENSOR_PRESION_H
#define SENSOR_PRESION_H

#include "Sensor.h"

class SensorPresion : public Sensor {
private:
    double presionMaxima;

public:
    SensorPresion(int _id, std::string _ub, double _lect, double _max = 120.0);
    void procesarLectura() override;
};

#endif
