#ifndef SENSOR_CAUDAL_H
#define SENSOR_CAUDAL_H

#include "Sensor.h"

class SensorCaudal : public Sensor {
private:
    double caudalMinimo;

public:
    SensorCaudal(int _id, std::string _ub, double _lect, double _min = 10.0);
    void procesarLectura() override;
};

#endif
