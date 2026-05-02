#include <iostream>
#include <sstream>
#include "Ticket.h"

Ticket::Ticket(const std::string& ticketNumber, const std::string& passengerId,
               const std::string& flightNumber, TicketClass ticketClass,
               double price, int freeBaggageAllowance)
    : ticketNumber(ticketNumber), passengerId(passengerId), flightNumber(flightNumber),
      ticketClass(ticketClass), issueDate(std::chrono::system_clock::now()),
      checkedIn(false), checkInType(CheckInType::Online), boarded(false),
      seatNumber(0), price(price), freeBaggageAllowance(freeBaggageAllowance) {
    std::cout << "\033[1;31mСоздан Ticket\033[0m\n";
}

Ticket::~Ticket() {
    std::cout << "\033[1;31mУдален Ticket\033[0m\n";
}

void Ticket::checkIn(CheckInType type) {
    if (checkedIn) {
        throw CheckInAlreadyCompletedException(ticketNumber);
    }
    checkInType = type;
    checkedIn = true;
    checkInTime = std::chrono::system_clock::now();
}

void Ticket::board() {
    if (!checkedIn) {
        throw CheckInNotCompletedException(ticketNumber);
    }
    if (boarded) {
        return;
    }
    boarded = true;
}

void Ticket::assignSeat(int seatNumber) {
    this->seatNumber = seatNumber;
}

void Ticket::displayInfo() const {
    std::cout << ticketNumber << " - ";
    std::cout << "Билет " << getClassString() << "\n";
    std::cout << "Пассажир: " << passengerId << "\n";
    std::cout << "Рейс: " << flightNumber << "\n";
    std::cout << "Место: " << seatNumber << "\n";
    std::cout << "Цена: " << price << " руб\n";
    std::cout << "Бесплатный багаж: " << freeBaggageAllowance << " кг\n";
    std::cout << "Регистрация: " << (checkedIn ? (checkInType == CheckInType::Online ? "онлайн" : "в аэропорту") : "не выполнена") << "\n";
    std::cout << "Посадка: " << (boarded ? "выполнена" : "не выполнена") << "\n";
}

std::string Ticket::getClassString() const {
    switch (ticketClass) {
        case TicketClass::Economy: return "Эконом";
        case TicketClass::Business: return "Бизнес";
        case TicketClass::First: return "Первый";
        default: return "Неизвестно";
    }
}

std::string Ticket::toJson() const {
    std::ostringstream oss;
    oss << ticketNumber << "|" << passengerId << "|" << flightNumber << "|";
    oss << (int)ticketClass << "|";
    
    std::time_t t = std::chrono::system_clock::to_time_t(issueDate);
    oss << t << "|";
    
    oss << (checkedIn ? 1 : 0) << "|" << (int)checkInType << "|";
    
    if (checkedIn) {
        t = std::chrono::system_clock::to_time_t(checkInTime);
        oss << t << "|";
    } else {
        oss << "0|";
    }
    
    oss << (boarded ? 1 : 0) << "|" << seatNumber << "|";
    oss << price << "|" << freeBaggageAllowance;
    
    return oss.str();
}

Ticket* Ticket::fromJson(const std::string& json) {
    std::istringstream iss(json);
    std::string ticketNumber, passengerId, flightNumber;
    std::string line;
    int ticketClassInt, checkedInInt, checkInTypeInt, seatNumber, boardedInt;
    std::time_t issueTime, checkInTime;
    double price;
    int freeBaggageAllowance;
    
    std::getline(iss, ticketNumber, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, flightNumber, '|');
    iss >> ticketClassInt; std::getline(iss, line, '|');
    iss >> issueTime; std::getline(iss, line, '|');
    iss >> checkedInInt; std::getline(iss, line, '|');
    iss >> checkInTypeInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> boardedInt; std::getline(iss, line, '|');
    iss >> seatNumber; std::getline(iss, line, '|');
    iss >> price; std::getline(iss, line, '|');
    iss >> freeBaggageAllowance;
    
    TicketClass ticketClass = (TicketClass)ticketClassInt;
    Ticket* ticket = nullptr;
    
    if (ticketClass == TicketClass::Economy) {
        ticket = new EconomyTicket(ticketNumber, passengerId, flightNumber, price);
    } else if (ticketClass == TicketClass::Business) {
        ticket = new BusinessTicket(ticketNumber, passengerId, flightNumber, price);
    } else if (ticketClass == TicketClass::First) {
        ticket = new FirstClassTicket(ticketNumber, passengerId, flightNumber, price);
    }
    
    ticket->issueDate = std::chrono::system_clock::from_time_t(issueTime);
    ticket->checkedIn = checkedInInt == 1;
    ticket->checkInType = (CheckInType)checkInTypeInt;
    if (ticket->checkedIn) {
        ticket->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    }
    ticket->boarded = boardedInt == 1;
    ticket->seatNumber = seatNumber;
    ticket->freeBaggageAllowance = freeBaggageAllowance;
    
    return ticket;
}

EconomyTicket::EconomyTicket(const std::string& ticketNumber, const std::string& passengerId,
                             const std::string& flightNumber, double price)
    : Ticket(ticketNumber, passengerId, flightNumber, TicketClass::Economy, price, 23) {
    std::cout << "\033[1;31mСоздан EconomyTicket\033[0m\n";
}

EconomyTicket::~EconomyTicket() {
    std::cout << "\033[1;31mУдален EconomyTicket\033[0m\n";
}

std::string EconomyTicket::toJson() const {
    return Ticket::toJson() + "|Economy";
}

EconomyTicket* EconomyTicket::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string ticketNumber, passengerId, flightNumber;
    std::string line;
    int ticketClassInt, checkedInInt, checkInTypeInt, seatNumber, boardedInt;
    std::time_t issueTime, checkInTime;
    double price;
    int freeBaggageAllowance;
    
    std::getline(iss, ticketNumber, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, flightNumber, '|');
    iss >> ticketClassInt; std::getline(iss, line, '|');
    iss >> issueTime; std::getline(iss, line, '|');
    iss >> checkedInInt; std::getline(iss, line, '|');
    iss >> checkInTypeInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> boardedInt; std::getline(iss, line, '|');
    iss >> seatNumber; std::getline(iss, line, '|');
    iss >> price; std::getline(iss, line, '|');
    iss >> freeBaggageAllowance;
    
    EconomyTicket* ticket = new EconomyTicket(ticketNumber, passengerId, flightNumber, price);
    ticket->issueDate = std::chrono::system_clock::from_time_t(issueTime);
    ticket->checkedIn = checkedInInt == 1;
    ticket->checkInType = (CheckInType)checkInTypeInt;
    if (ticket->checkedIn) {
        ticket->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    }
    ticket->boarded = boardedInt == 1;
    ticket->seatNumber = seatNumber;
    ticket->freeBaggageAllowance = freeBaggageAllowance;
    
    return ticket;
}

BusinessTicket::BusinessTicket(const std::string& ticketNumber, const std::string& passengerId,
                               const std::string& flightNumber, double price)
    : Ticket(ticketNumber, passengerId, flightNumber, TicketClass::Business, price, 32) {
    std::cout << "\033[1;31mСоздан BusinessTicket\033[0m\n";
}

BusinessTicket::~BusinessTicket() {
    std::cout << "\033[1;31mУдален BusinessTicket\033[0m\n";
}

std::string BusinessTicket::toJson() const {
    return Ticket::toJson() + "|Business";
}

BusinessTicket* BusinessTicket::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string ticketNumber, passengerId, flightNumber;
    std::string line;
    int ticketClassInt, checkedInInt, checkInTypeInt, seatNumber, boardedInt;
    std::time_t issueTime, checkInTime;
    double price;
    int freeBaggageAllowance;
    
    std::getline(iss, ticketNumber, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, flightNumber, '|');
    iss >> ticketClassInt; std::getline(iss, line, '|');
    iss >> issueTime; std::getline(iss, line, '|');
    iss >> checkedInInt; std::getline(iss, line, '|');
    iss >> checkInTypeInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> boardedInt; std::getline(iss, line, '|');
    iss >> seatNumber; std::getline(iss, line, '|');
    iss >> price; std::getline(iss, line, '|');
    iss >> freeBaggageAllowance;
    
    BusinessTicket* ticket = new BusinessTicket(ticketNumber, passengerId, flightNumber, price);
    ticket->issueDate = std::chrono::system_clock::from_time_t(issueTime);
    ticket->checkedIn = checkedInInt == 1;
    ticket->checkInType = (CheckInType)checkInTypeInt;
    if (ticket->checkedIn) {
        ticket->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    }
    ticket->boarded = boardedInt == 1;
    ticket->seatNumber = seatNumber;
    ticket->freeBaggageAllowance = freeBaggageAllowance;
    
    return ticket;
}

FirstClassTicket::FirstClassTicket(const std::string& ticketNumber, const std::string& passengerId,
                                   const std::string& flightNumber, double price)
    : Ticket(ticketNumber, passengerId, flightNumber, TicketClass::First, price, 40) {
    std::cout << "\033[1;31mСоздан FirstClassTicket\033[0m\n";
}

FirstClassTicket::~FirstClassTicket() {
    std::cout << "\033[1;31mУдален FirstClassTicket\033[0m\n";
}

std::string FirstClassTicket::toJson() const {
    return Ticket::toJson() + "|First";
}

FirstClassTicket* FirstClassTicket::fromJson(const std::string& json) {
    size_t pos = json.rfind('|');
    if (pos == std::string::npos) return nullptr;
    std::string baseJson = json.substr(0, pos);
    
    std::istringstream iss(baseJson);
    std::string ticketNumber, passengerId, flightNumber;
    std::string line;
    int ticketClassInt, checkedInInt, checkInTypeInt, seatNumber, boardedInt;
    std::time_t issueTime, checkInTime;
    double price;
    int freeBaggageAllowance;
    
    std::getline(iss, ticketNumber, '|');
    std::getline(iss, passengerId, '|');
    std::getline(iss, flightNumber, '|');
    iss >> ticketClassInt; std::getline(iss, line, '|');
    iss >> issueTime; std::getline(iss, line, '|');
    iss >> checkedInInt; std::getline(iss, line, '|');
    iss >> checkInTypeInt; std::getline(iss, line, '|');
    iss >> checkInTime; std::getline(iss, line, '|');
    iss >> boardedInt; std::getline(iss, line, '|');
    iss >> seatNumber; std::getline(iss, line, '|');
    iss >> price; std::getline(iss, line, '|');
    iss >> freeBaggageAllowance;
    
    FirstClassTicket* ticket = new FirstClassTicket(ticketNumber, passengerId, flightNumber, price);
    ticket->issueDate = std::chrono::system_clock::from_time_t(issueTime);
    ticket->checkedIn = checkedInInt == 1;
    ticket->checkInType = (CheckInType)checkInTypeInt;
    if (ticket->checkedIn) {
        ticket->checkInTime = std::chrono::system_clock::from_time_t(checkInTime);
    }
    ticket->boarded = boardedInt == 1;
    ticket->seatNumber = seatNumber;
    ticket->freeBaggageAllowance = freeBaggageAllowance;
    
    return ticket;
}
