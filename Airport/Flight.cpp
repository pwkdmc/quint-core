#include <iostream>
#include <sstream>
#include <iomanip>
#include "Flight.h"

Flight::Flight(const std::string& flightNumber, const std::string& destination,
               const std::string& origin, const std::chrono::system_clock::time_point& scheduledDeparture,
               const std::string& aircraftType, int totalSeats)
    : flightNumber(flightNumber), destination(destination), origin(origin),
      scheduledDeparture(scheduledDeparture), actualDeparture(scheduledDeparture),
      status(FlightStatus::Scheduled), totalSeats(totalSeats),
      availableSeats(totalSeats), delayMinutes(0), aircraftType(aircraftType) {
    std::cout << "\033[1;31mСоздан Flight\033[0m\n";
}

Flight::~Flight() {
    std::cout << "\033[1;31mУдален Flight\033[0m\n";
}

void Flight::setStatus(FlightStatus status) {
    this->status = status;
}

void Flight::delay(int minutes, const std::string& reason) {
    if (status == FlightStatus::Departed || status == FlightStatus::BoardingComplete) {
        throw AirportInputException("Рейс уже отправлен, задержка невозможна");
    }
    delayMinutes = minutes;
    delayReason = reason;
    status = FlightStatus::Delayed;
    scheduledDeparture += std::chrono::minutes(minutes);
}

void Flight::cancel() {
    if (status == FlightStatus::Departed || status == FlightStatus::BoardingComplete) {
        throw AirportInputException("Рейс уже отправлен, отмена невозможна");
    }
    status = FlightStatus::Cancelled;
}

void Flight::startBoarding() {
    if (status == FlightStatus::Departed || status == FlightStatus::BoardingComplete) {
        throw AirportInputException("Посадка уже завершена или рейс отправлен");
    }
    status = FlightStatus::Boarding;
}

void Flight::completeBoarding() {
    if (status != FlightStatus::Boarding) {
        throw AirportInputException("Посадка должна быть начата перед завершением");
    }
    status = FlightStatus::BoardingComplete;
}

void Flight::depart() {
    if (status != FlightStatus::BoardingComplete) {
        throw AirportInputException("Посадка должна быть завершена перед отправлением");
    }
    status = FlightStatus::Departed;
    actualDeparture = std::chrono::system_clock::now();
}

bool Flight::addPassenger(const std::string& passengerId) {
    if (availableSeats <= 0) {
        return false;
    }
    for (const auto& id : passengerIds) {
        if (id == passengerId) return false;
    }
    passengerIds.push_back(passengerId);
    availableSeats--;
    return true;
}

bool Flight::removePassenger(const std::string& passengerId) {
    for (size_t i = 0; i < passengerIds.size(); ++i) {
        if (passengerIds[i] == passengerId) {
            passengerIds.erase(passengerIds.begin() + i);
            availableSeats++;
            return true;
        }
    }
    return false;
}

bool Flight::isPassengerOnBoard(const std::string& passengerId) const {
    for (const auto& id : passengerIds) {
        if (id == passengerId) return true;
    }
    return false;
}

std::string Flight::getStatusString() const {
    switch (status) {
        case FlightStatus::Scheduled: return "По расписанию";
        case FlightStatus::Delayed: return "Задержан";
        case FlightStatus::Boarding: return "Посадка";
        case FlightStatus::BoardingComplete: return "Посадка завершена";
        case FlightStatus::Departed: return "Отправлен";
        case FlightStatus::Cancelled: return "Отменен";
        default: return "Неизвестно";
    }
}

void Flight::displayInfo() const {
    std::cout << flightNumber << " - " << destination << " (" << getStatusString() << ")\n";
    std::cout << "Самолет: " << aircraftType << "\n";
    
    std::time_t t = std::chrono::system_clock::to_time_t(scheduledDeparture);
    std::tm* tm = std::localtime(&t);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", tm);
    std::cout << "Время: " << buffer << "\n";
    
    std::cout << "Мест: " << (totalSeats - availableSeats) << "/" << totalSeats << "\n";
    if (status == FlightStatus::Delayed) {
        std::cout << "Задержка: " << delayMinutes << " мин (" << delayReason << ")\n";
    }
}

std::string Flight::toJson() const {
    std::ostringstream oss;
    oss << flightNumber << "|" << destination << "|" << origin << "|";
    
    std::time_t t = std::chrono::system_clock::to_time_t(scheduledDeparture);
    oss << t << "|";
    
    t = std::chrono::system_clock::to_time_t(actualDeparture);
    oss << t << "|";
    
    oss << (int)status << "|";
    
    for (size_t i = 0; i < passengerIds.size(); ++i) {
        if (i > 0) oss << ",";
        oss << passengerIds[i];
    }
    oss << "|";
    
    oss << totalSeats << "|" << availableSeats << "|";
    oss << delayMinutes << "|" << delayReason << "|" << aircraftType;
    
    return oss.str();
}

Flight* Flight::fromJson(const std::string& json) {
    std::istringstream iss(json);
    std::string flightNumber, destination, origin, passengerList;
    std::time_t schedTime, actualTime;
    int statusInt, totalSeats, availableSeats, delayMinutes;
    std::string delayReason, aircraftType;
    
    std::getline(iss, flightNumber, '|');
    std::getline(iss, destination, '|');
    std::getline(iss, origin, '|');
    iss >> schedTime; iss.ignore(1000, '|');
    iss >> actualTime; iss.ignore(1000, '|');
    iss >> statusInt; iss.ignore(1000, '|');
    std::getline(iss, passengerList, '|');
    iss >> totalSeats; iss.ignore(1000, '|');
    iss >> availableSeats; iss.ignore(1000, '|');
    iss >> delayMinutes; iss.ignore(1000, '|');
    std::getline(iss, delayReason, '|');
    std::getline(iss, aircraftType, '|');
    
    Flight* flight = new Flight(flightNumber, destination, origin,
                                std::chrono::system_clock::from_time_t(schedTime),
                                aircraftType, totalSeats);
    flight->actualDeparture = std::chrono::system_clock::from_time_t(actualTime);
    flight->status = (FlightStatus)statusInt;
    flight->availableSeats = availableSeats;
    flight->delayMinutes = delayMinutes;
    flight->delayReason = delayReason;
    if (delayMinutes > 0 && statusInt == (int)FlightStatus::Delayed) {
        flight->scheduledDeparture += std::chrono::minutes(delayMinutes);
    }
    
    if (!passengerList.empty()) {
        std::istringstream pss(passengerList);
        std::string pid;
        while (std::getline(pss, pid, ',')) {
            flight->passengerIds.push_back(pid);
        }
    }
    
    return flight;
}

InternationalFlight::InternationalFlight(const std::string& flightNumber, const std::string& destination,
                                         const std::chrono::system_clock::time_point& scheduledDeparture,
                                         const std::string& visaRequired, const std::string& aircraftType,
                                         int totalSeats)
    : Flight(flightNumber, destination, "Москва", scheduledDeparture, aircraftType, totalSeats),
      visaRequired(visaRequired) {
    std::cout << "\033[1;31mСоздан InternationalFlight\033[0m\n";
}

InternationalFlight::~InternationalFlight() {
    std::cout << "\033[1;31mУдален InternationalFlight\033[0m\n";
}

void InternationalFlight::displayInfo() const {
    Flight::displayInfo();
    std::cout << "Тип: Международный\n";
    std::cout << "Виза: " << visaRequired << "\n";
}

std::string InternationalFlight::toJson() const {
    return Flight::toJson() + "|International|" + visaRequired;
}

InternationalFlight* InternationalFlight::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(json.substr(pos + 1));
    std::string type, visaRequired;
    std::getline(iss, type, '|');
    std::getline(iss, visaRequired, '|');
    
    std::istringstream iss2(baseJson);
    std::string flightNumber, destination, origin, passengerList;
    std::time_t schedTime, actualTime;
    int statusInt, totalSeats, availableSeats, delayMinutes;
    std::string delayReason, aircraftType;
    
    std::getline(iss2, flightNumber, '|');
    std::getline(iss2, destination, '|');
    std::getline(iss2, origin, '|');
    iss2 >> schedTime; iss2.ignore(1000, '|');
    iss2 >> actualTime; iss2.ignore(1000, '|');
    iss2 >> statusInt; iss2.ignore(1000, '|');
    std::getline(iss2, passengerList, '|');
    iss2 >> totalSeats; iss2.ignore(1000, '|');
    iss2 >> availableSeats; iss2.ignore(1000, '|');
    iss2 >> delayMinutes; iss2.ignore(1000, '|');
    std::getline(iss2, delayReason, '|');
    std::getline(iss2, aircraftType, '|');
    
    InternationalFlight* flight = new InternationalFlight(flightNumber, destination,
                                                          std::chrono::system_clock::from_time_t(schedTime),
                                                          visaRequired, aircraftType, totalSeats);
    flight->actualDeparture = std::chrono::system_clock::from_time_t(actualTime);
    flight->status = (FlightStatus)statusInt;
    flight->availableSeats = availableSeats;
    flight->delayMinutes = delayMinutes;
    flight->delayReason = delayReason;
    if (delayMinutes > 0 && statusInt == (int)FlightStatus::Delayed) {
        flight->scheduledDeparture += std::chrono::minutes(delayMinutes);
    }
    
    if (!passengerList.empty()) {
        std::istringstream pss(passengerList);
        std::string pid;
        while (std::getline(pss, pid, ',')) {
            flight->passengerIds.push_back(pid);
        }
    }
    
    return flight;
}

DomesticFlight::DomesticFlight(const std::string& flightNumber, const std::string& destination,
                               const std::chrono::system_clock::time_point& scheduledDeparture,
                               const std::string& aircraftType, int totalSeats)
    : Flight(flightNumber, destination, "Москва", scheduledDeparture, aircraftType, totalSeats) {
    std::cout << "\033[1;31mСоздан DomesticFlight\033[0m\n";
}

DomesticFlight::~DomesticFlight() {
    std::cout << "\033[1;31mУдален DomesticFlight\033[0m\n";
}

void DomesticFlight::displayInfo() const {
    Flight::displayInfo();
    std::cout << "Тип: Внутренний\n";
}

std::string DomesticFlight::toJson() const {
    return Flight::toJson() + "|Domestic";
}

DomesticFlight* DomesticFlight::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(json.substr(pos + 1));
    std::string type;
    std::getline(iss, type, '|');
    
    std::istringstream iss2(baseJson);
    std::string flightNumber, destination, origin, passengerList;
    std::time_t schedTime, actualTime;
    int statusInt, totalSeats, availableSeats, delayMinutes;
    std::string delayReason, aircraftType;
    
    std::getline(iss2, flightNumber, '|');
    std::getline(iss2, destination, '|');
    std::getline(iss2, origin, '|');
    iss2 >> schedTime; iss2.ignore(1000, '|');
    iss2 >> actualTime; iss2.ignore(1000, '|');
    iss2 >> statusInt; iss2.ignore(1000, '|');
    std::getline(iss2, passengerList, '|');
    iss2 >> totalSeats; iss2.ignore(1000, '|');
    iss2 >> availableSeats; iss2.ignore(1000, '|');
    iss2 >> delayMinutes; iss2.ignore(1000, '|');
    std::getline(iss2, delayReason, '|');
    std::getline(iss2, aircraftType, '|');
    
    DomesticFlight* flight = new DomesticFlight(flightNumber, destination,
                                                std::chrono::system_clock::from_time_t(schedTime),
                                                aircraftType, totalSeats);
    flight->actualDeparture = std::chrono::system_clock::from_time_t(actualTime);
    flight->status = (FlightStatus)statusInt;
    flight->availableSeats = availableSeats;
    flight->delayMinutes = delayMinutes;
    flight->delayReason = delayReason;
    if (delayMinutes > 0 && statusInt == (int)FlightStatus::Delayed) {
        flight->scheduledDeparture += std::chrono::minutes(delayMinutes);
    }
    
    if (!passengerList.empty()) {
        std::istringstream pss(passengerList);
        std::string pid;
        while (std::getline(pss, pid, ',')) {
            flight->passengerIds.push_back(pid);
        }
    }
    
    return flight;
}
