#ifndef CARSHARING_CUSTOMER_H
#define CARSHARING_CUSTOMER_H

#include <string>
#include <vector>
#include "Car.h"
#include <iostream>

class Customer {
private:
    std::string id;
    std::string name;
    int age;
    int licenseYears;
    double bonusPoints;
    std::vector<RentalHistory> rentalHistory;
    
public:
    Customer(const std::string& id, const std::string& name, int age, int licenseYears)
        : id(id), name(name), age(age), licenseYears(licenseYears), bonusPoints(0.0) {
        std::cout << "\033[1;31mСоздан Customer\033[0m\n";
    }
    
    ~Customer() {
        std::cout << "\033[1;31mУдален Customer\033[0m\n";
    }
    
    int getAge() const { return age; }
    int getLicenseYears() const { return licenseYears; }
    double getBonusPoints() const { return bonusPoints; }
    std::string getName() const { return name; }
    std::string getId() const { return id; }
    
    void addBonusPoints(double points) {
        bonusPoints += points;
    }
    
    void useBonusPoints(double points) {
        if (points > bonusPoints) {
            points = bonusPoints;
        }
        bonusPoints -= points;
    }
    
    void addRentalRecord(const RentalHistory& record) {
        rentalHistory.push_back(record);
    }
    
    const std::vector<RentalHistory>& getRentalHistory() const {
        return rentalHistory;
    }
    
    void displayInfo() const {
        std::cout << "= Информация о клиенте =\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Имя: " << name << "\n";
        std::cout << "Возраст: " << age << " лет\n";
        std::cout << "Стаж вождения: " << licenseYears << " лет\n";
        std::cout << "Бонусные баллы: " << bonusPoints << "\n";
        std::cout << "Количество аренд: " << rentalHistory.size() << "\n";
    }
};

#endif
