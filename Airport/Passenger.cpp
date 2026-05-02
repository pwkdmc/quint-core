#include <iostream>
#include <sstream>
#include "Passenger.h"

Passenger::Passenger(const std::string& id, const std::string& firstName,
                     const std::string& lastName, const std::string& passportNumber,
                     int age, PassengerType type)
    : id(id), firstName(firstName), lastName(lastName), passportNumber(passportNumber),
      age(age), type(type), priorityLevel(1) {
    std::cout << "\033[1;31mСоздан Passenger\033[0m\n";
}

Passenger::~Passenger() {
    std::cout << "\033[1;31mУдален Passenger\033[0m\n";
}

void Passenger::addTicket(const std::string& ticketNumber) {
    for (const auto& t : ticketNumbers) {
        if (t == ticketNumber) return;
    }
    ticketNumbers.push_back(ticketNumber);
}

void Passenger::addLuggage(const std::string& luggageTag) {
    for (const auto& l : luggageTags) {
        if (l == luggageTag) return;
    }
    luggageTags.push_back(luggageTag);
}

void Passenger::removeTicket(const std::string& ticketNumber) {
    for (size_t i = 0; i < ticketNumbers.size(); ++i) {
        if (ticketNumbers[i] == ticketNumber) {
            ticketNumbers.erase(ticketNumbers.begin() + i);
            return;
        }
    }
}

void Passenger::removeLuggage(const std::string& luggageTag) {
    for (size_t i = 0; i < luggageTags.size(); ++i) {
        if (luggageTags[i] == luggageTag) {
            luggageTags.erase(luggageTags.begin() + i);
            return;
        }
    }
}

void Passenger::displayInfo() const {
    std::cout << id << " - " << firstName << " " << lastName << " (" << getTypeString() << ")\n";
    std::cout << "Паспорт: " << passportNumber << "\n";
    std::cout << "Возраст: " << age << "\n";
    std::cout << "Приоритет: " << priorityLevel << "\n";
    std::cout << "Билетов: " << ticketNumbers.size() << "\n";
    std::cout << "Багажа: " << luggageTags.size() << "\n";
}

std::string Passenger::getTypeString() const {
    switch (type) {
        case PassengerType::Economy: return "Эконом";
        case PassengerType::Business: return "Бизнес";
        case PassengerType::VIP: return "VIP";
        default: return "Неизвестно";
    }
}

std::string Passenger::toJson() const {
    std::ostringstream oss;
    oss << id << "|" << firstName << "|" << lastName << "|" << passportNumber << "|";
    oss << age << "|" << (int)type << "|";
    
    for (size_t i = 0; i < ticketNumbers.size(); ++i) {
        if (i > 0) oss << ",";
        oss << ticketNumbers[i];
    }
    oss << "|";
    
    for (size_t i = 0; i < luggageTags.size(); ++i) {
        if (i > 0) oss << ",";
        oss << luggageTags[i];
    }
    oss << "|" << priorityLevel;
    
    return oss.str();
}

Passenger* Passenger::fromJson(const std::string& json) {
    std::istringstream iss(json);
    std::string id, firstName, lastName, passportNumber, ticketList, luggageList;
    std::string line;
    int age, typeInt, priorityLevel;
    
    std::getline(iss, id, '|');
    std::getline(iss, firstName, '|');
    std::getline(iss, lastName, '|');
    std::getline(iss, passportNumber, '|');
    iss >> age; std::getline(iss, line, '|');
    iss >> typeInt; std::getline(iss, line, '|');
    std::getline(iss, ticketList, '|');
    std::getline(iss, luggageList, '|');
    iss >> priorityLevel;
    
    PassengerType type = (PassengerType)typeInt;
    Passenger* passenger = nullptr;
    
    if (type == PassengerType::Economy) {
        passenger = new EconomyPassenger(id, firstName, lastName, passportNumber, age);
    } else if (type == PassengerType::Business) {
        passenger = new BusinessPassenger(id, firstName, lastName, passportNumber, age);
    } else if (type == PassengerType::VIP) {
        passenger = new VIPPassenger(id, firstName, lastName, passportNumber, age);
    }
    
    if (!ticketList.empty()) {
        std::istringstream tss(ticketList);
        std::string tid;
        while (std::getline(tss, tid, ',')) {
            passenger->ticketNumbers.push_back(tid);
        }
    }
    
    if (!luggageList.empty()) {
        std::istringstream lss(luggageList);
        std::string lid;
        while (std::getline(lss, lid, ',')) {
            passenger->luggageTags.push_back(lid);
        }
    }
    
    passenger->priorityLevel = priorityLevel;
    
    return passenger;
}

int Passenger::getFreeBaggageAllowance() const {
    return 20;
}

double Passenger::getBaggageOverweightRate() const {
    return 10.0;
}

EconomyPassenger::EconomyPassenger(const std::string& id, const std::string& firstName,
                                   const std::string& lastName, const std::string& passportNumber, int age)
    : Passenger(id, firstName, lastName, passportNumber, age, PassengerType::Economy) {
    std::cout << "\033[1;31mСоздан EconomyPassenger\033[0m\n";
}

EconomyPassenger::~EconomyPassenger() {
    std::cout << "\033[1;31mУдален EconomyPassenger\033[0m\n";
}

std::string EconomyPassenger::getTypeString() const {
    return "Эконом";
}

int EconomyPassenger::getFreeBaggageAllowance() const {
    return 20;
}

double EconomyPassenger::getBaggageOverweightRate() const {
    return 10.0;
}

std::string EconomyPassenger::toJson() const {
    return Passenger::toJson() + "|Economy";
}

EconomyPassenger* EconomyPassenger::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string id, firstName, lastName, passportNumber, ticketList, luggageList;
    std::string line;
    int age, typeInt, priorityLevel;
    
    std::getline(iss, id, '|');
    std::getline(iss, firstName, '|');
    std::getline(iss, lastName, '|');
    std::getline(iss, passportNumber, '|');
    iss >> age; std::getline(iss, line, '|');
    iss >> typeInt; std::getline(iss, line, '|');
    std::getline(iss, ticketList, '|');
    std::getline(iss, luggageList, '|');
    iss >> priorityLevel;
    
    EconomyPassenger* passenger = new EconomyPassenger(id, firstName, lastName, passportNumber, age);
    passenger->priorityLevel = priorityLevel;
    
    if (!ticketList.empty()) {
        std::istringstream tss(ticketList);
        std::string tid;
        while (std::getline(tss, tid, ',')) {
            passenger->ticketNumbers.push_back(tid);
        }
    }
    
    if (!luggageList.empty()) {
        std::istringstream lss(luggageList);
        std::string lid;
        while (std::getline(lss, lid, ',')) {
            passenger->luggageTags.push_back(lid);
        }
    }
    
    return passenger;
}

BusinessPassenger::BusinessPassenger(const std::string& id, const std::string& firstName,
                                     const std::string& lastName, const std::string& passportNumber, int age)
    : Passenger(id, firstName, lastName, passportNumber, age, PassengerType::Business) {
    std::cout << "\033[1;31mСоздан BusinessPassenger\033[0m\n";
    priorityLevel = 2;
}

BusinessPassenger::~BusinessPassenger() {
    std::cout << "\033[1;31mУдален BusinessPassenger\033[0m\n";
}

std::string BusinessPassenger::getTypeString() const {
    return "Бизнес";
}

int BusinessPassenger::getFreeBaggageAllowance() const {
    return 32;
}

double BusinessPassenger::getBaggageOverweightRate() const {
    return 8.0;
}

std::string BusinessPassenger::toJson() const {
    return Passenger::toJson() + "|Business";
}

BusinessPassenger* BusinessPassenger::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string id, firstName, lastName, passportNumber, ticketList, luggageList;
    std::string line;
    int age, typeInt, priorityLevel;
    
    std::getline(iss, id, '|');
    std::getline(iss, firstName, '|');
    std::getline(iss, lastName, '|');
    std::getline(iss, passportNumber, '|');
    iss >> age; std::getline(iss, line, '|');
    iss >> typeInt; std::getline(iss, line, '|');
    std::getline(iss, ticketList, '|');
    std::getline(iss, luggageList, '|');
    iss >> priorityLevel;
    
    BusinessPassenger* passenger = new BusinessPassenger(id, firstName, lastName, passportNumber, age);
    passenger->priorityLevel = priorityLevel;
    
    if (!ticketList.empty()) {
        std::istringstream tss(ticketList);
        std::string tid;
        while (std::getline(tss, tid, ',')) {
            passenger->ticketNumbers.push_back(tid);
        }
    }
    
    if (!luggageList.empty()) {
        std::istringstream lss(luggageList);
        std::string lid;
        while (std::getline(lss, lid, ',')) {
            passenger->luggageTags.push_back(lid);
        }
    }
    
    return passenger;
}

VIPPassenger::VIPPassenger(const std::string& id, const std::string& firstName,
                           const std::string& lastName, const std::string& passportNumber, int age)
    : Passenger(id, firstName, lastName, passportNumber, age, PassengerType::VIP) {
    std::cout << "\033[1;31mСоздан VIPPassenger\033[0m\n";
    priorityLevel = 3;
}

VIPPassenger::~VIPPassenger() {
    std::cout << "\033[1;31mУдален VIPPassenger\033[0m\n";
}

std::string VIPPassenger::getTypeString() const {
    return "VIP";
}

int VIPPassenger::getFreeBaggageAllowance() const {
    return 40;
}

double VIPPassenger::getBaggageOverweightRate() const {
    return 5.0;
}

std::string VIPPassenger::toJson() const {
    return Passenger::toJson() + "|VIP";
}

VIPPassenger* VIPPassenger::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string id, firstName, lastName, passportNumber, ticketList, luggageList;
    std::string line;
    int age, typeInt, priorityLevel;
    
    std::getline(iss, id, '|');
    std::getline(iss, firstName, '|');
    std::getline(iss, lastName, '|');
    std::getline(iss, passportNumber, '|');
    iss >> age; std::getline(iss, line, '|');
    iss >> typeInt; std::getline(iss, line, '|');
    std::getline(iss, ticketList, '|');
    std::getline(iss, luggageList, '|');
    iss >> priorityLevel;
    
    VIPPassenger* passenger = new VIPPassenger(id, firstName, lastName, passportNumber, age);
    passenger->priorityLevel = priorityLevel;
    
    if (!ticketList.empty()) {
        std::istringstream tss(ticketList);
        std::string tid;
        while (std::getline(tss, tid, ',')) {
            passenger->ticketNumbers.push_back(tid);
        }
    }
    
    if (!luggageList.empty()) {
        std::istringstream lss(luggageList);
        std::string lid;
        while (std::getline(lss, lid, ',')) {
            passenger->luggageTags.push_back(lid);
        }
    }
    
    return passenger;
}
