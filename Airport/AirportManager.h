#ifndef AIRPORT_MANAGER_H
#define AIRPORT_MANAGER_H

#include <vector>
#include <string>
#include <chrono>
#include "Flight.h"
#include "Passenger.h"
#include "Ticket.h"
#include "Luggage.h"

class AirportManager {
private:
    std::vector<Flight*> flights;
    std::vector<Passenger*> passengers;
    std::vector<Ticket*> tickets;
    std::vector<Luggage*> luggage;
    std::string dataFile;
    
    void displayFlightsMenu();
    void displayPassengersMenu();
    void displayTicketsMenu();
    void displayLuggageMenu();
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getValidatedString(const std::string& prompt);
    std::chrono::system_clock::time_point parseDateTime(const std::string& input);
    std::string formatDateTime(const std::chrono::system_clock::time_point& date) const;
    int getNextFlightId();
    int getNextPassengerId();
    int getNextTicketId();
    int getNextLuggageId();
    bool saveToFile() const;
    bool loadFromFile();
    
    Flight* findFlightByNumber(const std::string& flightNumber) const;
    Passenger* findPassengerById(const std::string& passengerId) const;
    Ticket* findTicketByNumber(const std::string& ticketNumber) const;
    Luggage* findLuggageByTag(const std::string& luggageTag) const;
    
    void sortByBoardingPriority(std::vector<std::string>& passengerIds) const;

public:
    AirportManager();
    ~AirportManager();
    
    int run(int lastAction = 0);
    
private:
    void addFlight();
    void delayFlight();
    void cancelFlight();
    void addPassenger();
    void removePassenger();
    void createTicket();
    void deleteTicket();
    void checkInOnline();
    void checkInAirport();
    void addLuggage();
    void removeLuggage();
    void boardPassenger();
    void checkTransfer();
    void showFlightStatistics();
};

#endif
