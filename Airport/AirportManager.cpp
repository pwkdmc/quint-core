#include <iostream>
#include <limits>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "AirportManager.h"
#include "exceptions.h"

AirportManager::AirportManager() : dataFile("airport_data.txt") {
    std::cout << "\033[1;31mСоздан AirportManager\033[0m\n";
    loadFromFile();
}

AirportManager::~AirportManager() {
    saveToFile();
    
    for (size_t i = 0; i < flights.size(); ++i) {
        delete flights[i];
    }
    flights.clear();
    
    for (size_t i = 0; i < passengers.size(); ++i) {
        delete passengers[i];
    }
    passengers.clear();
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        delete tickets[i];
    }
    tickets.clear();
    
    for (size_t i = 0; i < luggage.size(); ++i) {
        delete luggage[i];
    }
    luggage.clear();
    
    std::cout << "\033[1;31mУдален AirportManager\033[0m\n";
}

int AirportManager::getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    std::cout << prompt;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw AirportInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (value < min || value > max) {
        throw AirportInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    
    return value;
}

std::string AirportManager::getValidatedString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty()) {
        throw AirportInputException("Поле не может быть пустым");
    }
    return value;
}

std::chrono::system_clock::time_point AirportManager::parseDateTime(const std::string& input) {
    std::tm tm = {};
    std::istringstream ss(input);
    ss.imbue(std::locale::classic());
    ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    
    if (ss.fail()) {
        throw AirportInputException("Неверный формат даты. Используйте ДД.ММ.ГГГГ ЧЧ:ММ");
    }
    
    ss >> std::ws;
    if (!ss.eof()) {
        throw AirportInputException("Неверный формат даты. Используйте ДД.ММ.ГГГГ ЧЧ:ММ");
    }
    
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string AirportManager::formatDateTime(const std::chrono::system_clock::time_point& date) const {
    std::time_t t = std::chrono::system_clock::to_time_t(date);
    std::tm* tm = std::localtime(&t);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", tm);
    return std::string(buffer);
}

Flight* AirportManager::findFlightByNumber(const std::string& flightNumber) const {
    for (size_t i = 0; i < flights.size(); ++i) {
        if (flights[i]->getFlightNumber() == flightNumber) {
            return flights[i];
        }
    }
    return nullptr;
}

Passenger* AirportManager::findPassengerById(const std::string& passengerId) const {
    for (size_t i = 0; i < passengers.size(); ++i) {
        if (passengers[i]->getId() == passengerId) {
            return passengers[i];
        }
    }
    return nullptr;
}

Ticket* AirportManager::findTicketByNumber(const std::string& ticketNumber) const {
    for (size_t i = 0; i < tickets.size(); ++i) {
        if (tickets[i]->getTicketNumber() == ticketNumber) {
            return tickets[i];
        }
    }
    return nullptr;
}

Luggage* AirportManager::findLuggageByTag(const std::string& luggageTag) const {
    for (size_t i = 0; i < luggage.size(); ++i) {
        if (luggage[i]->getLuggageTag() == luggageTag) {
            return luggage[i];
        }
    }
    return nullptr;
}

void AirportManager::sortByBoardingPriority(std::vector<std::string>& passengerIds) const {
    std::sort(passengerIds.begin(), passengerIds.end(), [this](const std::string& a, const std::string& b) {
        Passenger* pa = findPassengerById(a);
        Passenger* pb = findPassengerById(b);
        if (pa && pb) {
            return pa->getPriorityLevel() > pb->getPriorityLevel();
        }
        return false;
    });
}

bool AirportManager::saveToFile() const {
    std::ofstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "FLIGHT_COUNT:" << flights.size() << "\n";
    for (size_t i = 0; i < flights.size(); ++i) {
        file << "FLIGHT_START\n";
        file << "TYPE:" << (dynamic_cast<InternationalFlight*>(flights[i]) ? "International" : "Domestic") << "\n";
        file << flights[i]->toJson() << "\n";
        file << "FLIGHT_END\n";
    }
    
    file << "PASSENGER_COUNT:" << passengers.size() << "\n";
    for (size_t i = 0; i < passengers.size(); ++i) {
        file << "PASSENGER_START\n";
        file << "TYPE:" 
             << (dynamic_cast<VIPPassenger*>(passengers[i]) ? "VIP" : 
                 dynamic_cast<BusinessPassenger*>(passengers[i]) ? "Business" : "Economy") << "\n";
        file << passengers[i]->toJson() << "\n";
        file << "PASSENGER_END\n";
    }
    
    file << "TICKET_COUNT:" << tickets.size() << "\n";
    for (size_t i = 0; i < tickets.size(); ++i) {
        file << "TICKET_START\n";
        file << "TYPE:" 
             << (dynamic_cast<FirstClassTicket*>(tickets[i]) ? "First" :
                 dynamic_cast<BusinessTicket*>(tickets[i]) ? "Business" : "Economy") << "\n";
        file << tickets[i]->toJson() << "\n";
        file << "TICKET_END\n";
    }
    
    file << "LUGGAGE_COUNT:" << luggage.size() << "\n";
    for (size_t i = 0; i < luggage.size(); ++i) {
        file << "LUGGAGE_START\n";
        file << "TYPE:" << (dynamic_cast<CarryOnLuggage*>(luggage[i]) ? "CarryOn" : "Checked") << "\n";
        file << luggage[i]->toJson() << "\n";
        file << "LUGGAGE_END\n";
    }
    
    file.close();
    return true;
}

bool AirportManager::loadFromFile() {
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string section = "";
    std::string entityType = "";
    std::string data;
    
    while (std::getline(file, line)) {
        if (line == "FLIGHT_START") {
            section = "FLIGHT";
            entityType = "";
        } else if (line == "FLIGHT_END") {
            section = "";
        } else if (line == "PASSENGER_START") {
            section = "PASSENGER";
            entityType = "";
        } else if (line == "PASSENGER_END") {
            section = "";
        } else if (line == "TICKET_START") {
            section = "TICKET";
            entityType = "";
        } else if (line == "TICKET_END") {
            section = "";
        } else if (line == "LUGGAGE_START") {
            section = "LUGGAGE";
            entityType = "";
        } else if (line == "LUGGAGE_END") {
            section = "";
        } else if (line.find("TYPE:") == 0) {
            entityType = line.substr(5);
        } else if (!section.empty() && !line.empty()) {
            data = line;
            
            if (section == "FLIGHT") {
                if (entityType == "International") {
                    InternationalFlight* f = InternationalFlight::fromJson(data);
                    if (f) {
                        flights.push_back(f);
                    }
                } else {
                    DomesticFlight* f = DomesticFlight::fromJson(data);
                    if (f) {
                        flights.push_back(f);
                    }
                }
            } else if (section == "PASSENGER") {
                if (entityType == "VIP") {
                    VIPPassenger* p = VIPPassenger::fromJson(data);
                    if (p) {
                        passengers.push_back(p);
                    }
                } else if (entityType == "Business") {
                    BusinessPassenger* p = BusinessPassenger::fromJson(data);
                    if (p) {
                        passengers.push_back(p);
                    }
                } else {
                    EconomyPassenger* p = EconomyPassenger::fromJson(data);
                    if (p) {
                        passengers.push_back(p);
                    }
                }
            } else if (section == "TICKET") {
                if (entityType == "First") {
                    FirstClassTicket* t = FirstClassTicket::fromJson(data);
                    if (t) {
                        tickets.push_back(t);
                    }
                } else if (entityType == "Business") {
                    BusinessTicket* t = BusinessTicket::fromJson(data);
                    if (t) {
                        tickets.push_back(t);
                    }
                } else {
                    EconomyTicket* t = EconomyTicket::fromJson(data);
                    if (t) {
                        tickets.push_back(t);
                    }
                }
            } else if (section == "LUGGAGE") {
                if (entityType == "CarryOn") {
                    CarryOnLuggage* l = CarryOnLuggage::fromJson(data);
                    if (l) {
                        luggage.push_back(l);
                    }
                } else {
                    CheckedLuggage* l = CheckedLuggage::fromJson(data);
                    if (l) {
                        luggage.push_back(l);
                    }
                }
            }
            
            data = "";
        }
    }
    
    file.close();
    return true;
}

void AirportManager::displayFlightsMenu() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    std::cout << "= Список рейсов =\n";
    for (size_t i = 0; i < flights.size(); ++i) {
        std::cout << (i + 1) << ". ";
        flights[i]->displayInfo();
    }
}

void AirportManager::displayPassengersMenu() {
    if (passengers.empty()) {
        std::cout << "Пассажиров нет\n";
        return;
    }
    
    std::cout << "= Список пассажиров =\n";
    for (size_t i = 0; i < passengers.size(); ++i) {
        std::cout << (i + 1) << ". ";
        passengers[i]->displayInfo();
    }
}

void AirportManager::displayTicketsMenu() {
    if (tickets.empty()) {
        std::cout << "Билетов нет\n";
        return;
    }
    
    std::cout << "= Список билетов =\n";
    for (size_t i = 0; i < tickets.size(); ++i) {
        std::cout << (i + 1) << ". ";
        tickets[i]->displayInfo();
    }
}

void AirportManager::displayLuggageMenu() {
    if (luggage.empty()) {
        std::cout << "Багажа нет\n";
        return;
    }
    
    std::cout << "= Список багажа =\n";
    for (size_t i = 0; i < luggage.size(); ++i) {
        std::cout << (i + 1) << ". ";
        luggage[i]->displayInfo();
    }
}

void AirportManager::addFlight() {
    std::cout << "= Добавление рейса =\n";
    std::cout << "1. Внутренний рейс\n";
    std::cout << "2. Международный рейс\n";
    
    int typeChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите тип рейса:\n";
            typeChoice = getValidatedInt("", 1, 2);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int maxFlightId = 0;
    for (auto* f : flights) {
        std::string num = f->getFlightNumber();
        if (num.length() > 2) {
            try {
                int id = std::stoi(num.substr(2));
                if (id > maxFlightId) maxFlightId = id;
            } catch (...) {}
        }
    }
    std::string flightNumber = "SU" + std::to_string(maxFlightId + 1);
    std::string destination;
    while (true) {
        try {
            destination = getValidatedString("Введите пункт назначения:\n");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string departureStr;
    std::chrono::system_clock::time_point departureTime;
    while (true) {
        try {
            std::cout << "Введите время отправления (ДД.ММ.ГГГГ ЧЧ:ММ):\n";
            departureStr = getValidatedString("");
            departureTime = parseDateTime(departureStr);
            auto now = std::chrono::system_clock::now();
            if (departureTime <= now) {
                throw AirportInputException("Время отправления должно быть в будущем");
            }
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int totalSeats = 0;
    while (true) {
        try {
            std::cout << "Введите количество мест:\n";
            totalSeats = getValidatedInt("", 50, 500);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string aircraftType;
    while (true) {
        try {
            std::cout << "Введите тип самолета:\n";
            aircraftType = getValidatedString("");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = nullptr;
    if (typeChoice == 1) {
        flight = new DomesticFlight(flightNumber, destination, departureTime, aircraftType, totalSeats);
    } else {
        std::string visaRequired;
        while (true) {
            try {
                visaRequired = getValidatedString("Введите требования к визе:\n");
                break;
            } catch (const AirportException& e) {
                std::cerr << e.what() << "\n";
            }
        }
        flight = new InternationalFlight(flightNumber, destination, departureTime, visaRequired, aircraftType, totalSeats);
    }
    
    flights.push_back(flight);
    
    saveToFile();
}

void AirportManager::delayFlight() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    std::cout << "= Задержка рейса =\n";
    displayFlightsMenu();
    
    int flightChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер рейса:\n";
            flightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = flights[flightChoice - 1];
    
    if (flight->getStatus() == FlightStatus::Departed || 
        flight->getStatus() == FlightStatus::BoardingComplete ||
        flight->getStatus() == FlightStatus::Cancelled) {
        std::cout << "Рейс уже отправлен или отменен, задержка невозможна\n";
        return;
    }
    
    int delayMinutes = 0;
    while (true) {
        try {
            std::cout << "Введите задержку (минут):\n";
            delayMinutes = getValidatedInt("", 1, 480);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string reason;
    while (true) {
        try {
            std::cout << "Введите причину задержки:\n";
            reason = getValidatedString("");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    flight->delay(delayMinutes, reason);
    std::cout << "Рейс задержан на " << delayMinutes << " минут\n";
    
    saveToFile();
}

void AirportManager::cancelFlight() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    std::cout << "= Отмена рейса =\n";
    displayFlightsMenu();
    
    int flightChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер рейса:\n";
            flightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = flights[flightChoice - 1];
    
    if (flight->getStatus() == FlightStatus::Departed || 
        flight->getStatus() == FlightStatus::BoardingComplete) {
        std::cout << "Рейс уже отправлен или посадка завершена, отмена невозможна\n";
        return;
    }
    
    flight->cancel();
    std::cout << "Рейс отменен\n";
    
    saveToFile();
}

void AirportManager::addPassenger() {
    std::cout << "= Добавление пассажира =\n";
    std::cout << "1. Эконом\n";
    std::cout << "2. Бизнес\n";
    std::cout << "3. VIP\n";
    
    int typeChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите тип пассажира:\n";
            typeChoice = getValidatedInt("", 1, 3);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string firstName;
    while (true) {
        try {
            firstName = getValidatedString("Введите имя:\n");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string lastName;
    while (true) {
        try {
            lastName = getValidatedString("Введите фамилию:\n");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    std::string passport;
    while (true) {
        try {
            passport = getValidatedString("Введите номер паспорта:\n");
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int age = 0;
    while (true) {
        try {
            std::cout << "Введите возраст:\n";
            age = getValidatedInt("", 2, 120);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int maxPassengerId = 0;
    for (auto* p : passengers) {
        std::string id = p->getId();
        if (id.length() > 1 && id[0] == 'P') {
            try {
                int num = std::stoi(id.substr(1));
                if (num > maxPassengerId) maxPassengerId = num;
            } catch (...) {}
        }
    }
    std::string passengerId = "P" + std::to_string(maxPassengerId + 1);
    Passenger* passenger = nullptr;
    
    if (typeChoice == 1) {
        passenger = new EconomyPassenger(passengerId, firstName, lastName, passport, age);
    } else if (typeChoice == 2) {
        passenger = new BusinessPassenger(passengerId, firstName, lastName, passport, age);
    } else {
        passenger = new VIPPassenger(passengerId, firstName, lastName, passport, age);
    }
    
    passengers.push_back(passenger);
    std::cout << "Пассажир добавлен (ID: " << passengerId << ")\n";
    
    saveToFile();
}

void AirportManager::removePassenger() {
    if (passengers.empty()) {
        std::cout << "Пассажиров нет\n";
        return;
    }
    
    std::cout << "= Удаление пассажира =\n";
    displayPassengersMenu();
    
    int passengerChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер пассажира:\n";
            passengerChoice = getValidatedInt("", 1, passengers.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Passenger* passenger = passengers[passengerChoice - 1];
    std::string passengerId = passenger->getId();
    
    // Проверяем, не посажен ли пассажир на какой-либо рейс
    for (auto* t : tickets) {
        if (t->getPassengerId() == passengerId && t->getBoarded()) {
            std::cout << "Нельзя удалить пассажира: он уже посажен на рейс " 
                      << t->getFlightNumber() << "\n";
            return;
        }
    }
    
    for (size_t i = 0; i < tickets.size(); ) {
        if (tickets[i]->getPassengerId() == passengerId) {
            Flight* flight = findFlightByNumber(tickets[i]->getFlightNumber());
            if (flight) {
                flight->removePassenger(passengerId);
            }
            delete tickets[i];
            tickets.erase(tickets.begin() + i);
        } else {
            ++i;
        }
    }
    
    for (size_t i = 0; i < luggage.size(); ) {
        if (luggage[i]->getPassengerId() == passengerId) {
            delete luggage[i];
            luggage.erase(luggage.begin() + i);
        } else {
            ++i;
        }
    }
    
    delete passenger;
    passengers.erase(passengers.begin() + passengerChoice - 1);
    std::cout << "Пассажир удален\n";
    
    saveToFile();
}

void AirportManager::createTicket() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    if (passengers.empty()) {
        std::cout << "Пассажиров нет\n";
        return;
    }
    
    std::cout << "= Создание билета =\n";
    displayFlightsMenu();
    
    int flightChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер рейса:\n";
            flightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = flights[flightChoice - 1];
    
    if (flight->getStatus() == FlightStatus::BoardingComplete ||
        flight->getStatus() == FlightStatus::Departed ||
        flight->getStatus() == FlightStatus::Cancelled) {
        std::cout << "Нельзя создать билет на этот рейс\n";
        return;
    }
    
    displayPassengersMenu();
    int passengerChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер пассажира:\n";
            passengerChoice = getValidatedInt("", 1, passengers.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Passenger* passenger = passengers[passengerChoice - 1];
    
    for (auto* t : tickets) {
        if (t->getPassengerId() == passenger->getId() && 
            t->getFlightNumber() == flight->getFlightNumber()) {
            std::cout << "У этого пассажира уже есть билет на этот рейс\n";
            return;
        }
    }
    
    std::cout << "1. Эконом\n";
    std::cout << "2. Бизнес\n";
    std::cout << "3. Первый класс\n";
    
    int classChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите класс билета:\n";
            classChoice = getValidatedInt("", 1, 3);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    double price = 0;
    while (true) {
        try {
            std::cout << "Введите цену билета:\n";
            price = getValidatedInt("", 1000, 200000);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int maxTicketId = 0;
    for (auto* t : tickets) {
        std::string num = t->getTicketNumber();
        if (num.length() > 2) {
            try {
                int id = std::stoi(num.substr(2));
                if (id > maxTicketId) maxTicketId = id;
            } catch (...) {}
        }
    }
    std::string ticketNumber = "TK" + std::to_string(maxTicketId + 1);
    Ticket* ticket = nullptr;
    int baggageAllowance = 0;
    
    if (classChoice == 1) {
        ticket = new EconomyTicket(ticketNumber, passenger->getId(), flight->getFlightNumber(), price);
        baggageAllowance = 23;
    } else if (classChoice == 2) {
        ticket = new BusinessTicket(ticketNumber, passenger->getId(), flight->getFlightNumber(), price);
        baggageAllowance = 32;
    } else {
        ticket = new FirstClassTicket(ticketNumber, passenger->getId(), flight->getFlightNumber(), price);
        baggageAllowance = 40;
    }
    
    ticket->assignSeat(flight->getTotalSeats() - flight->getAvailableSeats() + 1);
    
    passenger->addTicket(ticketNumber);
    flight->addPassenger(passenger->getId());
    tickets.push_back(ticket);
    
    std::cout << "Билет создан (№ " << ticketNumber << ")\n";
    
    saveToFile();
}

void AirportManager::deleteTicket() {
    if (tickets.empty()) {
        std::cout << "Билетов нет\n";
        return;
    }
    
    std::cout << "= Удаление билета =\n";
    displayTicketsMenu();
    
    int ticketChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер билета:\n";
            ticketChoice = getValidatedInt("", 1, tickets.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Ticket* ticket = tickets[ticketChoice - 1];
    std::string ticketNumber = ticket->getTicketNumber();
    std::string passengerId = ticket->getPassengerId();
    std::string flightNumber = ticket->getFlightNumber();
    
    Passenger* passenger = findPassengerById(passengerId);
    if (passenger) {
        passenger->removeTicket(ticketNumber);
    }
    
    Flight* flight = findFlightByNumber(flightNumber);
    if (flight) {
        flight->removePassenger(passengerId);
    }
    
    for (size_t i = 0; i < luggage.size(); ) {
        if (luggage[i]->getTicketNumber() == ticketNumber) {
            delete luggage[i];
            luggage.erase(luggage.begin() + i);
        } else {
            ++i;
        }
    }
    
    delete ticket;
    tickets.erase(tickets.begin() + ticketChoice - 1);
    std::cout << "Билет удален\n";
    
    saveToFile();
}

void AirportManager::checkInOnline() {
    if (tickets.empty()) {
        std::cout << "Билетов нет\n";
        return;
    }
    
    std::cout << "= Онлайн регистрация =\n";
    displayTicketsMenu();
    
    int ticketChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер билета:\n";
            ticketChoice = getValidatedInt("", 1, tickets.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Ticket* ticket = tickets[ticketChoice - 1];
    
    if (ticket->getCheckedIn()) {
        std::cout << "Регистрация уже выполнена\n";
        return;
    }
    
    Flight* flight = findFlightByNumber(ticket->getFlightNumber());
    if (!flight) {
        std::cout << "Рейс не найден\n";
        return;
    }
    
    if (flight->getStatus() == FlightStatus::BoardingComplete ||
        flight->getStatus() == FlightStatus::Departed) {
        std::cout << "Регистрация закрыта\n";
        return;
    }
    
    ticket->checkIn(CheckInType::Online);
    std::cout << "Регистрация выполнена онлайн\n";
    
    saveToFile();
}

void AirportManager::checkInAirport() {
    if (tickets.empty()) {
        std::cout << "Билетов нет\n";
        return;
    }
    
    std::cout << "= Регистрация в аэропорту =\n";
    displayTicketsMenu();
    
    int ticketChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер билета:\n";
            ticketChoice = getValidatedInt("", 1, tickets.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Ticket* ticket = tickets[ticketChoice - 1];
    
    if (ticket->getCheckedIn()) {
        std::cout << "Регистрация уже выполнена\n";
        return;
    }
    
    Flight* flight = findFlightByNumber(ticket->getFlightNumber());
    if (!flight) {
        std::cout << "Рейс не найден\n";
        return;
    }
    
    if (flight->getStatus() == FlightStatus::BoardingComplete ||
        flight->getStatus() == FlightStatus::Departed) {
        std::cout << "Регистрация закрыта\n";
        return;
    }
    
    ticket->checkIn(CheckInType::Airport);
    std::cout << "Регистрация выполнена в аэропорту\n";
    
    saveToFile();
}

void AirportManager::addLuggage() {
    if (tickets.empty()) {
        std::cout << "Билетов нет\n";
        return;
    }
    
    std::cout << "= Добавление багажа =\n";
    displayTicketsMenu();
    
    int ticketChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер билета:\n";
            ticketChoice = getValidatedInt("", 1, tickets.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Ticket* ticket = tickets[ticketChoice - 1];
    Ticket* currentTicket = ticket;
    
    Passenger* passenger = findPassengerById(ticket->getPassengerId());
    if (passenger) {
        for (size_t i = 0; i < tickets.size(); ++i) {
            if (tickets[i]->getPassengerId() == passenger->getId() && 
                tickets[i]->getBoarded()) {
                std::cout << "Нельзя добавить багаж: пассажир уже посажен в самолет\n";
                return;
            }
        }
    }
    
    std::cout << "1. Ручная кладь\n";
    std::cout << "2. Сдаваемый багаж\n";
    std::cout << "Выберите тип багажа:\n";
    
    int typeChoice = 0;
    while (true) {
        try {
            typeChoice = getValidatedInt("", 1, 2);
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    double weight = 0;
    while (true) {
        try {
            std::cout << "Введите вес багажа (кг):\n";
            weight = std::stod(getValidatedString(""));
            if (weight <= 0 || weight > 100) {
                throw AirportInputException("Вес должен быть от 0 до 100 кг");
            }
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    int maxLuggageId = 0;
    for (auto* l : luggage) {
        std::string tag = l->getLuggageTag();
        if (tag.length() > 2) {
            try {
                int id = std::stoi(tag.substr(2));
                if (id > maxLuggageId) maxLuggageId = id;
            } catch (...) {}
        }
    }
    std::string luggageTag = "LG" + std::to_string(maxLuggageId + 1);
    Luggage* luggageItem = nullptr;
    
    if (typeChoice == 1) {
        luggageItem = new CarryOnLuggage(luggageTag, ticket->getPassengerId(), 
                                          ticket->getTicketNumber(), weight);
    } else {
        luggageItem = new CheckedLuggage(luggageTag, ticket->getPassengerId(),
                                          ticket->getTicketNumber(), weight);
        
        Passenger* passenger = findPassengerById(ticket->getPassengerId());
        if (passenger) {
            passenger->addLuggage(luggageTag);
        }
        
        double freeAllowance = currentTicket->getFreeBaggageAllowance();
        double overweightRate = passenger ? passenger->getBaggageOverweightRate() : 10.0;
        
        try {
            luggageItem->calculateOverweightFee(freeAllowance, overweightRate);
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    luggage.push_back(luggageItem);
    
    saveToFile();
}

void AirportManager::removeLuggage() {
    if (luggage.empty()) {
        std::cout << "Багажа нет\n";
        return;
    }
    
    std::cout << "= Удаление багажа =\n";
    displayLuggageMenu();
    
    int luggageChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер багажа:\n";
            luggageChoice = getValidatedInt("", 1, luggage.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Luggage* luggageItem = luggage[luggageChoice - 1];
    std::string luggageTag = luggageItem->getLuggageTag();
    
    Passenger* passenger = findPassengerById(luggageItem->getPassengerId());
    if (passenger) {
        passenger->removeLuggage(luggageTag);
    }
    
    delete luggageItem;
    luggage.erase(luggage.begin() + luggageChoice - 1);
    std::cout << "Багаж удален\n";
    
    saveToFile();
}

void AirportManager::boardPassenger() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    std::cout << "= Посадка пассажиров =\n";
    displayFlightsMenu();
    
    int flightChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер рейса:\n";
            flightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = flights[flightChoice - 1];
    
    if (flight->getStatus() == FlightStatus::Departed) {
        std::cout << "Рейс уже отправлен\n";
        return;
    }
    
    if (flight->getStatus() == FlightStatus::BoardingComplete) {
        std::cout << "Посадка уже завершена\n";
        return;
    }
    
    if (flight->getStatus() != FlightStatus::Boarding) {
        std::cout << "Посадка еще не началась. Начните посадку.\n";
        std::cout << "1. Начать посадку\n";
        std::cout << "0. Отмена\n";
        std::cout << "Выберите действие:\n";
        int action = getValidatedInt("", 0, 1);
        if (action == 0) return;
        flight->startBoarding();
        std::cout << "Посадка началась\n";
    }
    
    std::vector<std::string> passengersToBoard = flight->getPassengerIds();
    if (passengersToBoard.empty()) {
        std::cout << "Нет пассажиров на рейсе\n";
        std::cout << "1. Завершить посадку\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие:\n";
        int action = getValidatedInt("", 0, 1);
        if (action == 0) return;
        flight->completeBoarding();
        std::cout << "Посадка завершена\n";
        saveToFile();
        return;
    }
    
    sortByBoardingPriority(passengersToBoard);
    
    std::cout << "Порядок посадки (по приоритету):\n";
    for (size_t i = 0; i < passengersToBoard.size(); ++i) {
        Passenger* p = findPassengerById(passengersToBoard[i]);
        if (p) {
            bool isBoarded = false;
            for (size_t j = 0; j < tickets.size(); ++j) {
                if (tickets[j]->getPassengerId() == p->getId() && 
                    tickets[j]->getFlightNumber() == flight->getFlightNumber() && 
                    tickets[j]->getBoarded()) {
                    isBoarded = true;
                    break;
                }
            }
            std::cout << (i + 1) << ". " << p->getFullName() 
                      << " (" << p->getTypeString() << ")"
                      << (isBoarded ? " (посажен)" : "") << "\n";
        }
    }
    
    std::cout << "1. Посадить следующего пассажира\n";
    std::cout << "2. Завершить посадку\n";
    std::cout << "Выберите действие:\n";
    
    int action = getValidatedInt("", 1, 2);
    if (action == 1) {
        bool boarded = false;
        for (size_t i = 0; i < passengersToBoard.size(); ++i) {
            Passenger* p = findPassengerById(passengersToBoard[i]);
            if (p) {
                for (size_t j = 0; j < tickets.size(); ++j) {
                    if (tickets[j]->getPassengerId() == p->getId() && 
                        tickets[j]->getFlightNumber() == flight->getFlightNumber()) {
                        if (!tickets[j]->getBoarded()) {
                            tickets[j]->board();
                            std::cout << p->getFullName() << " посажен\n";
                            boarded = true;
                            break;
                        }
                    }
                }
            }
            if (boarded) break;
        }
        if (!boarded) {
            std::cout << "Все пассажиры уже посажены\n";
        }
    } else {
        flight->completeBoarding();
        std::cout << "Посадка завершена\n";
    }
    
    saveToFile();
}

void AirportManager::checkTransfer() {
    if (flights.size() < 2) {
        std::cout << "Недостаточно рейсов для проверки трансфера\n";
        return;
    }
    
    std::cout << "= Проверка трансфера =\n";
    displayFlightsMenu();
    
    int arrivalFlightChoice = 0;
    int departureFlightChoice = 0;
    
    while (true) {
        try {
            std::cout << "Выберите номер прибывающего рейса:\n";
            arrivalFlightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    while (true) {
        try {
            std::cout << "Выберите номер вылетающего рейса:\n";
            departureFlightChoice = getValidatedInt("", 1, flights.size());
            if (arrivalFlightChoice == departureFlightChoice) {
                throw AirportInputException("Рейсы должны быть разными");
            }
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* arrivalFlight = flights[arrivalFlightChoice - 1];
    Flight* departureFlight = flights[departureFlightChoice - 1];
    
    auto arrivalTime = arrivalFlight->getScheduledDeparture();
    auto departureTime = departureFlight->getScheduledDeparture();
    
    std::time_t t1 = std::chrono::system_clock::to_time_t(arrivalTime);
    std::tm* tm1 = std::localtime(&t1);
    char buffer1[30];
    std::strftime(buffer1, sizeof(buffer1), "%d.%m.%Y %H:%M", tm1);
    
    std::time_t t2 = std::chrono::system_clock::to_time_t(departureTime);
    std::tm* tm2 = std::localtime(&t2);
    char buffer2[30];
    std::strftime(buffer2, sizeof(buffer2), "%d.%m.%Y %H:%M", tm2);
    
    std::cout << "Прибытие: " << arrivalFlight->getFlightNumber() << " - " << buffer1;
    if (arrivalFlight->getStatus() == FlightStatus::Delayed) {
        std::cout << " (задержка " << arrivalFlight->getDelayMinutes() << " мин)";
    }
    std::cout << "\n";
    
    std::cout << "Вылет: " << departureFlight->getFlightNumber() << " - " << buffer2;
    if (departureFlight->getStatus() == FlightStatus::Delayed) {
        std::cout << " (задержка " << departureFlight->getDelayMinutes() << " мин)";
    }
    std::cout << "\n";
    
    auto diff = std::chrono::duration_cast<std::chrono::minutes>(departureTime - arrivalTime);
    int availableMinutes = (int)diff.count();
    
    std::cout << "Время на трансфер: " << availableMinutes << " мин\n";
    
    int recommendedMinutes = 45;
    if (dynamic_cast<InternationalFlight*>(arrivalFlight) || 
        dynamic_cast<InternationalFlight*>(departureFlight)) {
        recommendedMinutes = 120;
    }

    try {
        if (availableMinutes < recommendedMinutes) {
            throw ConnectionTimeInsufficientException(recommendedMinutes, availableMinutes);
        }
        std::cout << "Трансфер возможен\n";
    } catch (const ConnectionTimeInsufficientException &e) {
        std::cerr << e.what() << "\n";
    }
    
    saveToFile();
}

void AirportManager::showFlightStatistics() {
    if (flights.empty()) {
        std::cout << "Рейсов нет\n";
        return;
    }
    
    std::cout << "= Статистика рейсов =\n";
    displayFlightsMenu();
    
    int flightChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер рейса:\n";
            flightChoice = getValidatedInt("", 1, flights.size());
            break;
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Flight* flight = flights[flightChoice - 1];
    
    std::cout << "Статистика рейса " << flight->getFlightNumber() << ":\n";
    std::cout << "Пункт назначения: " << flight->getDestination() << "\n";
    std::cout << "Статус: " << flight->getStatusString() << "\n";
    std::cout << "Мест всего: " << flight->getTotalSeats() << "\n";
    std::cout << "Мест доступно: " << flight->getAvailableSeats() << "\n";
    
    int checkedIn = 0;
    int boarded = 0;
    int economyCount = 0;
    int businessCount = 0;
    int vipCount = 0;
    
    for (size_t i = 0; i < tickets.size(); ++i) {
        if (tickets[i]->getFlightNumber() == flight->getFlightNumber()) {
            if (tickets[i]->getCheckedIn()) checkedIn++;
            if (tickets[i]->getBoarded()) boarded++;
            
            if (dynamic_cast<EconomyTicket*>(tickets[i])) economyCount++;
            else if (dynamic_cast<BusinessTicket*>(tickets[i])) businessCount++;
            else if (dynamic_cast<FirstClassTicket*>(tickets[i])) vipCount++;
        }
    }
    
    std::cout << "Классы билетов:\n";
    std::cout << "Эконом: " << economyCount << "\n";
    std::cout << "Бизнес: " << businessCount << "\n";
    std::cout << "Первый: " << vipCount << "\n";
    
    std::cout << "Регистрация: " << checkedIn << " пассажиров\n";
    std::cout << "Посадка: " << boarded << " пассажиров\n";
    
    int luggageCount = 0;
    double totalLuggageWeight = 0;
    for (size_t i = 0; i < luggage.size(); ++i) {
        bool isForFlight = false;
        for (size_t j = 0; j < tickets.size(); ++j) {
            if (tickets[j]->getTicketNumber() == luggage[i]->getTicketNumber() &&
                tickets[j]->getFlightNumber() == flight->getFlightNumber()) {
                isForFlight = true;
                break;
            }
        }
        if (isForFlight) {
            luggageCount++;
            totalLuggageWeight += luggage[i]->getActualWeight();
        }
    }
    
    std::cout << "Багаж: " << luggageCount << " единиц\n";
    std::cout << "Общий вес: " << std::fixed << std::setprecision(1) << totalLuggageWeight << " кг\n";
    
    saveToFile();
}

int AirportManager::run(int lastAction) {
    bool taskComplete = false;
    int actionToRepeat = lastAction;
    
    while (!taskComplete) {
        if (lastAction == 0) {
            std::cout << "== АЭРОПОРТ ==\n";
            std::cout << "1. Добавить рейс\n";
            std::cout << "2. Задержать рейс\n";
            std::cout << "3. Отменить рейс\n";
            std::cout << "4. Добавить пассажира\n";
            std::cout << "5. Удалить пассажира\n";
            std::cout << "6. Создать билет\n";
            std::cout << "7. Удалить билет\n";
            std::cout << "8. Регистрация онлайн\n";
            std::cout << "9. Регистрация в аэропорту\n";
            std::cout << "10. Добавить багаж\n";
            std::cout << "11. Удалить багаж\n";
            std::cout << "12. Посадка пассажиров\n";
            std::cout << "13. Проверить трансфер\n";
            std::cout << "14. Статистика рейса\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие:\n";
        }
        
        try {
            if (lastAction == 0) {
                lastAction = getValidatedInt("", 0, 14);
            }
            
            switch (lastAction) {
                case 1:
                    addFlight();
                    taskComplete = true;
                    break;
                case 2:
                    delayFlight();
                    taskComplete = true;
                    break;
                case 3:
                    cancelFlight();
                    taskComplete = true;
                    break;
                case 4:
                    addPassenger();
                    taskComplete = true;
                    break;
                case 5:
                    removePassenger();
                    taskComplete = true;
                    break;
                case 6:
                    createTicket();
                    taskComplete = true;
                    break;
                case 7:
                    deleteTicket();
                    taskComplete = true;
                    break;
                case 8:
                    checkInOnline();
                    taskComplete = true;
                    break;
                case 9:
                    checkInAirport();
                    taskComplete = true;
                    break;
                case 10:
                    addLuggage();
                    taskComplete = true;
                    break;
                case 11:
                    removeLuggage();
                    taskComplete = true;
                    break;
                case 12:
                    boardPassenger();
                    taskComplete = true;
                    break;
                case 13:
                    checkTransfer();
                    taskComplete = true;
                    break;
                case 14:
                    showFlightStatistics();
                    taskComplete = true;
                    break;
                case 0:
                    return 0;
            }
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
            lastAction = actionToRepeat;
        }
    }
    
    return lastAction;
}
