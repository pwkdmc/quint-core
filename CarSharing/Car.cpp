#include "Car.h"
#include <sstream>
#include <iomanip>
#include <ctime>

double EconomyCar::calculateCost(const std::chrono::system_clock::time_point& startTime,
                                  const std::chrono::system_clock::time_point& endTime,
                                  Tariff* tariff) const {
    double baseCost = tariff->calculateCost(startTime, endTime);
    double categoryMultiplier = 0.8;
    return baseCost * categoryMultiplier;
}

double BusinessCar::calculateCost(const std::chrono::system_clock::time_point& startTime,
                                   const std::chrono::system_clock::time_point& endTime,
                                   Tariff* tariff) const {
    double baseCost = tariff->calculateCost(startTime, endTime);
    double categoryMultiplier = 2.5;
    return baseCost * categoryMultiplier;
}

double MinivanCar::calculateCost(const std::chrono::system_clock::time_point& startTime,
                                  const std::chrono::system_clock::time_point& endTime,
                                  Tariff* tariff) const {
    double baseCost = tariff->calculateCost(startTime, endTime);
    double categoryMultiplier = 1.5;
    return baseCost * categoryMultiplier;
}

double ElectricCar::calculateCost(const std::chrono::system_clock::time_point& startTime,
                                   const std::chrono::system_clock::time_point& endTime,
                                   Tariff* tariff) const {
    double baseCost = tariff->calculateCost(startTime, endTime);
    double categoryMultiplier = 1.2;
    return baseCost * categoryMultiplier;
}

std::string EconomyCar::toJson() const {
    std::ostringstream oss;
    oss << "ECONOMY|" << id << "|" << model << "|" << maxRentalHours;
    return oss.str();
}

std::string BusinessCar::toJson() const {
    std::ostringstream oss;
    oss << "BUSINESS|" << id << "|" << model << "|" << maxRentalHours;
    return oss.str();
}

std::string MinivanCar::toJson() const {
    std::ostringstream oss;
    oss << "MINIVAN|" << id << "|" << model << "|" << maxRentalHours;
    return oss.str();
}

std::string ElectricCar::toJson() const {
    std::ostringstream oss;
    oss << "ELECTRIC|" << id << "|" << model << "|" << maxRentalHours << "|" 
        << batteryCapacity << "|" << currentCharge;
    return oss.str();
}

Car* Car::fromJson(const std::string& json) {
    std::istringstream iss(json);
    std::string type;
    
    if (!std::getline(iss, type, '|')) {
        return nullptr;
    }
    
    std::string id, model;
    int maxRentalHours;
    
    if (!std::getline(iss, id, '|') || !std::getline(iss, model, '|') || !(iss >> maxRentalHours)) {
        return nullptr;
    }
    
    if (type == "ECONOMY") {
        return new EconomyCar(id, model);
    } else if (type == "BUSINESS") {
        return new BusinessCar(id, model);
    } else if (type == "MINIVAN") {
        return new MinivanCar(id, model);
    } else if (type == "ELECTRIC") {
        int batteryCapacity, currentCharge;
        if (iss >> batteryCapacity >> currentCharge) {
            return new ElectricCar(id, model, batteryCapacity, currentCharge);
        }
        return new ElectricCar(id, model);
    }
    
    return nullptr;
}

