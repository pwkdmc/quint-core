#ifndef AIRPORT_FLIGHT_H
#define AIRPORT_FLIGHT_H

#include <string>
#include <vector>
#include <chrono>
#include "exceptions.h"

enum class FlightStatus {
    Scheduled,
    Delayed,
    Boarding,
    BoardingComplete,
    Departed,
    Cancelled
};

class Flight {
protected:
    std::string flightNumber;
    std::string destination;
    std::string origin;
    std::chrono::system_clock::time_point scheduledDeparture;
    std::chrono::system_clock::time_point actualDeparture;
    FlightStatus status;
    std::vector<std::string> passengerIds;
    int totalSeats;
    int availableSeats;
    std::string delayReason;
    int delayMinutes;
    std::string aircraftType;

public:
    Flight(const std::string& flightNumber, const std::string& destination, 
           const std::string& origin, const std::chrono::system_clock::time_point& scheduledDeparture,
           const std::string& aircraftType, int totalSeats = 150);
    virtual ~Flight();
    
    std::string getFlightNumber() const { return flightNumber; }
    std::string getDestination() const { return destination; }
    std::string getOrigin() const { return origin; }
    std::chrono::system_clock::time_point getScheduledDeparture() const { return scheduledDeparture; }
    std::chrono::system_clock::time_point getActualDeparture() const { return actualDeparture; }
    FlightStatus getStatus() const { return status; }
    int getTotalSeats() const { return totalSeats; }
    int getAvailableSeats() const { return availableSeats; }
    int getDelayMinutes() const { return delayMinutes; }
    const std::string& getDelayReason() const { return delayReason; }
    const std::vector<std::string>& getPassengerIds() const { return passengerIds; }
    std::string getAircraftType() const { return aircraftType; }
    
    void setStatus(FlightStatus status);
    void delay(int minutes, const std::string& reason);
    void cancel();
    void startBoarding();
    void completeBoarding();
    void depart();
    
    bool addPassenger(const std::string& passengerId);
    bool removePassenger(const std::string& passengerId);
    bool isPassengerOnBoard(const std::string& passengerId) const;
    
    virtual void displayInfo() const;
    virtual std::string getStatusString() const;
    virtual std::string toJson() const;
    static Flight* fromJson(const std::string& json);
};

class InternationalFlight : public Flight {
private:
    std::string visaRequired;

public:
    InternationalFlight(const std::string& flightNumber, const std::string& destination,
                        const std::chrono::system_clock::time_point& scheduledDeparture,
                        const std::string& visaRequired = "N/A", const std::string& aircraftType = "Boeing 777",
                        int totalSeats = 200);
    ~InternationalFlight() override;
    
    std::string getVisaRequired() const { return visaRequired; }
    
    void displayInfo() const override;
    std::string toJson() const override;
    static InternationalFlight* fromJson(const std::string& json);
};

class DomesticFlight : public Flight {
public:
    DomesticFlight(const std::string& flightNumber, const std::string& destination,
                   const std::chrono::system_clock::time_point& scheduledDeparture,
                   const std::string& aircraftType = "Boeing 737", int totalSeats = 150);
    ~DomesticFlight() override;
    
    void displayInfo() const override;
    std::string toJson() const override;
    static DomesticFlight* fromJson(const std::string& json);
};

#endif
