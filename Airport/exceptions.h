#ifndef AIRPORT_EXCEPTIONS_H
#define AIRPORT_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class AirportException : public std::runtime_error {
public:
    explicit AirportException(const std::string& message) 
        : std::runtime_error(message) {}
};

class AirportInputException : public AirportException {
public:
    explicit AirportInputException(const std::string& message) 
        : AirportException("Ошибка: " + message) {}
};

class FlightNotFoundException : public AirportException {
public:
    explicit FlightNotFoundException(const std::string& flightNumber) 
        : AirportException("Ошибка: Рейс " + flightNumber + " не найден") {}
};

class PassengerNotFoundException : public AirportException {
public:
    explicit PassengerNotFoundException(const std::string& passengerId) 
        : AirportException("Ошибка: Пассажир " + passengerId + " не найден") {}
};

class TicketNotFoundException : public AirportException {
public:
    explicit TicketNotFoundException(const std::string& ticketNumber) 
        : AirportException("Ошибка: Билет " + ticketNumber + " не найден") {}
};

class LuggageNotFoundException : public AirportException {
public:
    explicit LuggageNotFoundException(const std::string& luggageTag) 
        : AirportException("Ошибка: Багаж " + luggageTag + " не найден") {}
};

class InvalidFlightStatusException : public AirportException {
public:
    explicit InvalidFlightStatusException(const std::string& status) 
        : AirportException("Ошибка: Неверный статус рейса " + status) {}
};

class FlightDelayedException : public AirportException {
public:
    explicit FlightDelayedException(const std::string& flightNumber, int delayMinutes) 
        : AirportException("Ошибка: Рейс " + flightNumber + " задержан на " + std::to_string(delayMinutes) + " мин") {}
};

class FlightCancelledException : public AirportException {
public:
    explicit FlightCancelledException(const std::string& flightNumber) 
        : AirportException("Ошибка: Рейс " + flightNumber + " отменен") {}
};

class CheckInAlreadyCompletedException : public AirportException {
public:
    explicit CheckInAlreadyCompletedException(const std::string& ticketNumber) 
        : AirportException("Ошибка: Регистрация на билет " + ticketNumber + " уже завершена") {}
};

class CheckInNotCompletedException : public AirportException {
public:
    explicit CheckInNotCompletedException(const std::string& ticketNumber) 
        : AirportException("Ошибка: Регистрация на билет " + ticketNumber + " не завершена") {}
};

class BoardingAlreadyCompletedException : public AirportException {
public:
    explicit BoardingAlreadyCompletedException(const std::string& flightNumber) 
        : AirportException("Ошибка: Посадка на рейс " + flightNumber + " уже завершена") {}
};

class BoardingNotStartedException : public AirportException {
public:
    explicit BoardingNotStartedException(const std::string& flightNumber) 
        : AirportException("Ошибка: Посадка на рейс " + flightNumber + " еще не началась") {}
};

class LuggageOverweightException : public AirportException {
public:
    explicit LuggageOverweightException(double overweight) 
        : AirportException("Ошибка: Превышение веса багажа на " + std::to_string(overweight) + " кг") {}
};

class InvalidPassengerTypeException : public AirportException {
public:
    explicit InvalidPassengerTypeException() 
        : AirportException("Ошибка: Неверный тип пассажира") {}
};

class ConnectionTimeInsufficientException : public AirportException {
public:
    explicit ConnectionTimeInsufficientException(int neededMinutes, int availableMinutes) 
        : AirportException("Ошибка: Необходимо " + std::to_string(neededMinutes) + " мин на трансфер, доступно " + std::to_string(availableMinutes) + " мин") {}
};

#endif
