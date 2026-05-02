#include <iostream>
#include <sstream>
#include "Luggage.h"

Luggage::Luggage(const std::string& luggageTag, const std::string& passengerId,
                 const std::string& ticketNumber, double weight)
    : luggageTag(luggageTag), passengerId(passengerId), ticketNumber(ticketNumber),
      weight(weight), actualWeight(weight), status(LuggageStatus::CheckedIn),
      overweightFee(0) {
    std::cout << "\033[1;31mСоздан Luggage\033[0m\n";
}

Luggage::~Luggage() {
    std::cout << "\033[1;31mУдален Luggage\033[0m\n";
}

void Luggage::setActualWeight(double weight) {
    this->actualWeight = weight;
}

void Luggage::setStatus(LuggageStatus status) {
    this->status = status;
}

double Luggage::calculateOverweightFee(double freeAllowance, double overweightRate) const {
    if (actualWeight > freeAllowance) {
        double overweight = actualWeight - freeAllowance;
        return overweight * overweightRate;
    }
    return 0;
}

void Luggage::displayInfo() const {
    std::cout << luggageTag << " - ";
    std::cout << "Багаж " << getStatusString() << "\n";
    std::cout << "Пассажир: " << passengerId << "\n";
    std::cout << "Билет: " << ticketNumber << "\n";
    std::cout << "Вес: " << actualWeight << " кг\n";
    if (overweightFee > 0) {
        std::cout << "Доплата за перевес: " << overweightFee << " руб\n";
    }
}

std::string Luggage::getStatusString() const {
    switch (status) {
        case LuggageStatus::CheckedIn: return "Сдан";
        case LuggageStatus::InTransit: return "В транзите";
        case LuggageStatus::OnBoard: return "На борту";
        case LuggageStatus::Delivered: return "Выдан";
        case LuggageStatus::Lost: return "Потерян";
        default: return "Неизвестно";
    }
}

std::string Luggage::toJson() const {
    std::ostringstream oss;
    oss << luggageTag << "|" << passengerId << "|" << ticketNumber << "|";
    oss << weight << "|" << actualWeight << "|";
    oss << (int)status << "|";
    
    std::time_t t = std::chrono::system_clock::to_time_t(checkInTime);
    oss << t << "|" << overweightFee;
    
    return oss.str();
}

Luggage* Luggage::fromJson(const std::string& json) {
    std::istringstream iss(json);
    std::string luggageTag, passengerId, ticketNumber;
    std::string line;
    double weight, actualWeight, overweightFee;
    int statusInt;
    std::time_t checkInTime;
    
    std::getline(iss, luggageTag, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, ticketNumber, '|');
    iss >> weight; std::getline(iss, line, '|');
    iss >> actualWeight; std::getline(iss, line, '|');
    iss >> statusInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> overweightFee;
    
    Luggage* luggage = nullptr;
    
    luggage = new Luggage(luggageTag, passengerId, ticketNumber, weight);
    luggage->actualWeight = actualWeight;
    luggage->status = (LuggageStatus)statusInt;
    luggage->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    luggage->overweightFee = overweightFee;
    
    return luggage;
}

CarryOnLuggage::CarryOnLuggage(const std::string& luggageTag, const std::string& passengerId,
                               const std::string& ticketNumber, double weight)
    : Luggage(luggageTag, passengerId, ticketNumber, weight), isCarried(false) {
    std::cout << "\033[1;31mСоздан CarryOnLuggage\033[0m\n";
}

CarryOnLuggage::~CarryOnLuggage() {
    std::cout << "\033[1;31mУдален CarryOnLuggage\033[0m\n";
}

void CarryOnLuggage::setCarried(bool carried) {
    isCarried = carried;
}

void CarryOnLuggage::displayInfo() const {
    Luggage::displayInfo();
    std::cout << "Тип: Ручная кладь\n";
    std::cout << "При себе: " << (isCarried ? "Да" : "Нет") << "\n";
}

std::string CarryOnLuggage::toJson() const {
    return Luggage::toJson() + "|CarryOn|" + (isCarried ? "1" : "0");
}

CarryOnLuggage* CarryOnLuggage::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string luggageTag, passengerId, ticketNumber;
    std::string line;
    double weight, actualWeight, overweightFee;
    int statusInt;
    std::time_t checkInTime;
    
    std::getline(iss, luggageTag, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, ticketNumber, '|');
    iss >> weight; std::getline(iss, line, '|');
    iss >> actualWeight; std::getline(iss, line, '|');
    iss >> statusInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> overweightFee;
    
    std::istringstream iss2(json.substr(pos + 1));
    std::string type, isCarriedStr;
    std::getline(iss2, type, '|');
    std::getline(iss2, isCarriedStr, '|');
    
    CarryOnLuggage* luggage = new CarryOnLuggage(luggageTag, passengerId, ticketNumber, weight);
    luggage->actualWeight = actualWeight;
    luggage->status = (LuggageStatus)statusInt;
    luggage->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    luggage->overweightFee = overweightFee;
    luggage->isCarried = isCarriedStr == "1";
    
    return luggage;
}

CheckedLuggage::CheckedLuggage(const std::string& luggageTag, const std::string& passengerId,
                               const std::string& ticketNumber, double weight)
    : Luggage(luggageTag, passengerId, ticketNumber, weight),
      currentLocation("Check-in") {
    std::cout << "\033[1;31mСоздан CheckedLuggage\033[0m\n";
}

CheckedLuggage::~CheckedLuggage() {
    std::cout << "\033[1;31mУдален CheckedLuggage\033[0m\n";
}

void CheckedLuggage::setCurrentLocation(const std::string& location) {
    currentLocation = location;
}

void CheckedLuggage::displayInfo() const {
    Luggage::displayInfo();
    std::cout << "Тип: Сдаваемый багаж\n";
    std::cout << "Текущее место: " << currentLocation << "\n";
}

std::string CheckedLuggage::toJson() const {
    return Luggage::toJson() + "|Checked|" + currentLocation;
}

CheckedLuggage* CheckedLuggage::fromJson(const std::string& json) {
    size_t lastPos = json.rfind('|');
    if (lastPos == std::string::npos) return nullptr;
    size_t secondLastPos = json.rfind('|', lastPos - 1);
    if (secondLastPos == std::string::npos) return nullptr;
    
    std::string baseJson = json.substr(0, secondLastPos);
    
    std::istringstream iss(baseJson);
    std::string luggageTag, passengerId, ticketNumber;
    std::string line;
    double weight, actualWeight, overweightFee;
    int statusInt;
    std::time_t checkInTime;
    
    std::getline(iss, luggageTag, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, ticketNumber, '|');
    iss >> weight; std::getline(iss, line, '|');
    iss >> actualWeight; std::getline(iss, line, '|');
    iss >> statusInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> overweightFee;
    
    std::istringstream iss2(json.substr(secondLastPos + 1));
    std::string type, currentLocation;
    std::getline(iss2, type, '|');
    std::getline(iss2, currentLocation, '|');
    
    CheckedLuggage* luggage = new CheckedLuggage(luggageTag, passengerId, ticketNumber, weight);
    luggage->actualWeight = actualWeight;
    luggage->status = (LuggageStatus)statusInt;
    luggage->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    luggage->overweightFee = overweightFee;
    luggage->currentLocation = currentLocation;
    
    return luggage;
}
