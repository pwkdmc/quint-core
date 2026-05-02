#ifndef CARSHARING_CAR_H
#define CARSHARING_CAR_H

#include <string>
#include <vector>
#include <chrono>
#include "exceptions.h"
#include "Tariff.h"
#include <iostream>

struct RentalHistory {
    std::string customerId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double cost;
    std::string tariffType;
};

class Car {
protected:
    std::string id;
    std::string model;
    std::string category;
    std::vector<RentalHistory> history;
    int maxRentalHours;
    
public:
    Car(const std::string& id, const std::string& model, const std::string& category,
        int maxRentalHours = 24)
        : id(id), model(model), category(category), maxRentalHours(maxRentalHours) {
        std::cout << "\033[1;31mСоздан Car\033[0m\n";
    }
    
    virtual ~Car() {
        std::cout << "\033[1;31mУдален Car\033[0m\n";
    }
    
    virtual double calculateCost(const std::chrono::system_clock::time_point& startTime,
                                  const std::chrono::system_clock::time_point& endTime,
                                  Tariff* tariff) const = 0;
    
    virtual std::string getCarType() const = 0;
    
    virtual int getMinAge() const = 0;
    virtual int getMinLicenseYears() const = 0;
    
    std::string getId() const { return id; }
    std::string getModel() const { return model; }
    std::string getCategory() const { return category; }
    
    void addHistoryRecord(const RentalHistory& record) {
        history.push_back(record);
    }
    
    const std::vector<RentalHistory>& getHistory() const {
        return history;
    }
    
    bool isBusyAtTime(const std::chrono::system_clock::time_point& start,
                      const std::chrono::system_clock::time_point& end) const {
        for (const auto& record : history) {
            if (start < record.endTime && end > record.startTime) {
                return true;
            }
        }
        return false;
    }
    
    int getMaxRentalHours() const { return maxRentalHours; }
    
    void displayInfo() const {
        std::cout << "ID: " << id << "\n";
        std::cout << "Модель: " << model << "\n";
        std::cout << "Категория: " << category << "\n";
        std::cout << "Макс. время аренды: " << maxRentalHours << " часов\n";
        std::cout << "Мин. возраст: " << getMinAge() << " лет\n";
        std::cout << "Мин. стаж: " << getMinLicenseYears() << " год\n";
    }
    
    virtual std::string toJson() const = 0;
    static Car* fromJson(const std::string& json);
};

class EconomyCar : public Car {
public:
    EconomyCar(const std::string& id, const std::string& model)
        : Car(id, model, "Эконом", 12) {
        std::cout << "\033[1;31mСоздан EconomyCar\033[0m\n";
    }
    
    ~EconomyCar() override {
        std::cout << "\033[1;31mУдален EconomyCar\033[0m\n";
    }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime,
                         Tariff* tariff) const override;
    
    std::string getCarType() const override { return "Эконом"; }
    int getMinAge() const override { return 21; }
    int getMinLicenseYears() const override { return 1; }
    
    std::string toJson() const override;
};

class BusinessCar : public Car {
public:
    BusinessCar(const std::string& id, const std::string& model)
        : Car(id, model, "Бизнес", 18) {
        std::cout << "\033[1;31mСоздан BusinessCar\033[0m\n";
    }
    
    ~BusinessCar() override {
        std::cout << "\033[1;31mУдален BusinessCar\033[0m\n";
    }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime,
                         Tariff* tariff) const override;
    
    std::string getCarType() const override { return "Бизнес"; }
    int getMinAge() const override { return 25; }
    int getMinLicenseYears() const override { return 3; }
    
    std::string toJson() const override;
};

class MinivanCar : public Car {
public:
    MinivanCar(const std::string& id, const std::string& model)
        : Car(id, model, "Минивэн", 24) {
        std::cout << "\033[1;31mСоздан MinivanCar\033[0m\n";
    }
    
    ~MinivanCar() override {
        std::cout << "\033[1;31mУдален MinivanCar\033[0m\n";
    }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime,
                         Tariff* tariff) const override;
    
    std::string getCarType() const override { return "Минивэн"; }
    int getMinAge() const override { return 23; }
    int getMinLicenseYears() const override { return 2; }
    
    std::string toJson() const override;
};

class ElectricCar : public Car {
private:
    int batteryCapacity;
    int currentCharge;
    
public:
    ElectricCar(const std::string& id, const std::string& model, 
                int batteryCapacity = 60, int currentCharge = 100)
        : Car(id, model, "Электромобиль", 8), 
          batteryCapacity(batteryCapacity), currentCharge(currentCharge) {
        std::cout << "\033[1;31mСоздан ElectricCar\033[0m\n";
    }
    
    ~ElectricCar() override {
        std::cout << "\033[1;31mУдален ElectricCar\033[0m\n";
    }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime,
                         Tariff* tariff) const override;
    
    std::string getCarType() const override { return "Электромобиль"; }
    int getMinAge() const override { return 22; }
    int getMinLicenseYears() const override { return 1; }
    
    int getCurrentCharge() const { return currentCharge; }
    
    std::string toJson() const override;
};

#endif
