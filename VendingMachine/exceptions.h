#ifndef VENDING_MACHINE_EXCEPTIONS_H
#define VENDING_MACHINE_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class VendingMachineException : public std::runtime_error {
public:
    explicit VendingMachineException(const std::string& message) 
        : std::runtime_error(message) {}
};

class VendingMachineInvalidProductTypeException : public VendingMachineException {
public:
    explicit VendingMachineInvalidProductTypeException() 
        : VendingMachineException("Ошибка: Неверный тип товара") {}
};

class VendingMachineInsufficientFundsException : public VendingMachineException {
public:
    explicit VendingMachineInsufficientFundsException() 
        : VendingMachineException("Ошибка: Недостаточно средств") {}
};

class VendingMachineInvalidPaymentException : public VendingMachineException {
public:
    explicit VendingMachineInvalidPaymentException(const std::string& message) 
        : VendingMachineException("Ошибка: " + message) {}
};

class VendingMachineProductExpiredException : public VendingMachineException {
public:
    explicit VendingMachineProductExpiredException() 
        : VendingMachineException("Ошибка: Товар просрочен") {}
};

class VendingMachineProductNotFoundException : public VendingMachineException {
public:
    explicit VendingMachineProductNotFoundException(const std::string& productName) 
        : VendingMachineException("Ошибка: Товар " + productName + " не найден") {}
};

class VendingMachineOutOfStockException : public VendingMachineException {
public:
    explicit VendingMachineOutOfStockException(const std::string& productName) 
        : VendingMachineException("Ошибка: Товар " + productName + " закончился") {}
};

class VendingMachineInvalidInputException : public VendingMachineException {
public:
    explicit VendingMachineInvalidInputException(const std::string& message) 
        : VendingMachineException("Ошибка: " + message) {}
};

class VendingMachineInvalidDateException : public VendingMachineException {
public:
    explicit VendingMachineInvalidDateException(const std::string& message) 
        : VendingMachineException("Ошибка: " + message) {}
};

class VendingMachineMaintenanceException : public VendingMachineException {
public:
    explicit VendingMachineMaintenanceException() 
        : VendingMachineException("Ошибка: Автомат в режиме техобслуживания") {}
};

#endif
