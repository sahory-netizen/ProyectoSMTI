#ifndef EXCEPCIONES_SMTI_H
#define EXCEPCIONES_SMTI_H

#include <stdexcept>
#include <string>

// Errores de socket o puerto (socket, bind, listen, accept)
class ErrorRed : public std::runtime_error {
public:
    ErrorRed(const std::string& msg) : std::runtime_error(msg) {}
};

// Trama mal formada o repetida
class ErrorTrama : public std::runtime_error {
public:
    ErrorTrama(const std::string& msg) : std::runtime_error(msg) {}
};

#endif
