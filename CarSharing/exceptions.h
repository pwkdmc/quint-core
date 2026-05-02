#ifndef CARSHARING_EXCEPTIONS_H
#define CARSHARING_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class CarSharingException : public std::runtime_error {
public:
    explicit CarSharingException(const std::string& message) 
        : std::runtime_error(message) {}
};

class InvalidAgeException : public CarSharingException {
public:
    explicit InvalidAgeException() 
        : CarSharingException("Ошибка: Недостаточный возраст") {}
};

class InvalidLicenseException : public CarSharingException {
public:
    explicit InvalidLicenseException() 
        : CarSharingException("Ошибка: Недостаточный стаж вождения") {}
};

class CarUnavailableException : public CarSharingException {
public:
    explicit CarUnavailableException(const std::string& carId) 
        : CarSharingException("Ошибка: Автомобиль " + carId + " недоступен в указанный период") {}
};

class InvalidTariffException : public CarSharingException {
public:
    explicit InvalidTariffException() 
        : CarSharingException("Ошибка: Неверный ID тарифа") {}
};

class RentalTimeLimitExceededException : public CarSharingException {
public:
    explicit RentalTimeLimitExceededException() 
        : CarSharingException("Ошибка: Превышен лимит аренды") {}
};

class InvalidRentalTimeException : public CarSharingException {
public:
    explicit InvalidRentalTimeException() 
        : CarSharingException("Ошибка: Неверное время аренды") {}
};

class InvalidDateTimeException : public CarSharingException {
public:
    explicit InvalidDateTimeException(const std::string& message) 
        : CarSharingException("Ошибка: " + message) {}
};

class InvalidInputException : public CarSharingException {
public:
    explicit InvalidInputException(const std::string& message) 
        : CarSharingException("Ошибка: " + message) {}
};

class CustomerUnavailableException : public CarSharingException {
public:
    explicit CustomerUnavailableException(const std::string& customerId) 
        : CarSharingException("Ошибка: Клиент " + customerId + " не найден") {}
};

#endif
