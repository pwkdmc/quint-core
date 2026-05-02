#ifndef METRO_EXCEPTIONS_H
#define METRO_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class MetroException : public std::runtime_error {
public:
    explicit MetroException(const std::string& message) 
        : std::runtime_error(message) {}
};

class MetroInputException : public MetroException {
public:
    explicit MetroInputException(const std::string& message) 
        : MetroException("Ошибка: " + message) {}
};

class StationNotFoundException : public MetroException {
public:
    explicit StationNotFoundException(const std::string& stationName) 
        : MetroException("Ошибка: Станция " + stationName + " не найдена") {}
};

class LineNotFoundException : public MetroException {
public:
    explicit LineNotFoundException(const std::string& lineName) 
        : MetroException("Ошибка: Линия " + lineName + " не найдена") {}
};

class RouteNotFoundException : public MetroException {
public:
    explicit RouteNotFoundException(const std::string& from, const std::string& to) 
        : MetroException("Ошибка: Маршрут от " + from + " до " + to + " не найден") {}
};

class StationClosedException : public MetroException {
public:
    explicit StationClosedException(const std::string& stationName) 
        : MetroException("Ошибка: Станция " + stationName + " закрыта") {}
};

class TransferNotAllowedException : public MetroException {
public:
    explicit TransferNotAllowedException(const std::string& stationName) 
        : MetroException("Ошибка: Пересадка на станции " + stationName + " недоступна") {}
};

#endif
