#ifndef CARSHARING_RENTAL_H
#define CARSHARING_RENTAL_H

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "Car.h"
#include "Tariff.h"
#include "Customer.h"
#include <iostream>

class Rental {
private:
    Car* car;
    Customer* customer;
    Tariff* tariff;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    int rentalMinutes;
    double totalCost;
    
    std::string formatDateTime(const std::chrono::system_clock::time_point& time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        std::tm tm = *std::localtime(&time_t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d.%m.%Y %H:%M");
        return oss.str();
    }
    
public:
    Rental(Car* car, Customer* customer, Tariff* tariff, 
           std::chrono::system_clock::time_point startTime,
           std::chrono::system_clock::time_point endTime)
        : car(car), customer(customer), tariff(tariff), 
          startTime(startTime), endTime(endTime), rentalMinutes(0), totalCost(0.0) {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        rentalMinutes = static_cast<int>(duration.count());
        std::cout << "\033[1;31mСоздан Rental\033[0m\n";
    }
    
    ~Rental() {
        std::cout << "\033[1;31mУдален Rental\033[0m\n";
    }
    
    void processRental() {
        std::cout << "Начало аренды: " << formatDateTime(startTime) << "\n";
        std::cout << "Конец аренды: " << formatDateTime(endTime) << "\n";
        
        double baseCost = car->calculateCost(startTime, endTime, tariff);
        
        std::cout << "Базовая стоимость: " << baseCost << " руб\n";
        
        totalCost = baseCost;
        
        double bonusEarned = totalCost * 0.05;
        customer->addBonusPoints(bonusEarned);
        std::cout << "Начислено бонусов: " << bonusEarned << "\n";
        std::cout << "Текущие бонусы клиента: " << customer->getBonusPoints() << "\n";
        
        double bonusUsed = customer->getBonusPoints() * 0.1;
        if (bonusUsed > 0) {
            std::cout << "Скидка за бонусы: " << bonusUsed << " руб\n";
            customer->useBonusPoints(bonusUsed);
            totalCost -= bonusUsed;
        }
        
        std::cout << "Итоговые бонусы после аренды: " << customer->getBonusPoints() << "\n";
        
        RentalHistory record;
        record.customerId = customer->getId();
        record.startTime = startTime;
        record.endTime = endTime;
        record.cost = totalCost;
        record.tariffType = tariff->getType();
        
        car->addHistoryRecord(record);
        customer->addRentalRecord(record);
    }
    
    double getTotalCost() const { return totalCost; }
    
    void displayReceipt() const {
        std::cout << "= Чек аренды =\n";
        std::cout << "Автомобиль: " << car->getModel() << " (" << car->getCarType() << ")\n";
        std::cout << "Клиент: " << customer->getName() << "\n";
        std::cout << "Тариф: " << tariff->getType() << "\n";
        std::cout << "Период аренды: " << formatDateTime(startTime) << " - " << formatDateTime(endTime) << "\n";
        std::cout << "Время аренды: " << rentalMinutes << " минут\n";
        std::cout << "Итоговая стоимость: " << totalCost << " руб\n";
    }
};

#endif
