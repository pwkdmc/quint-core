#ifndef AIRPORT_TICKET_H
#define AIRPORT_TICKET_H

#include <string>
#include <chrono>
#include "exceptions.h"

enum class TicketClass {
    Economy,
    Business,
    First
};

enum class CheckInType {
    Online,
    Airport
};

class Ticket {
protected:
    std::string ticketNumber;
    std::string passengerId;
    std::string flightNumber;
    TicketClass ticketClass;
    std::chrono::system_clock::time_point issueDate;
    bool checkedIn;
    CheckInType checkInType;
    std::chrono::system_clock::time_point checkInTime;
    bool boarded;
    int seatNumber;
    double price;
    int freeBaggageAllowance;

public:
    Ticket(const std::string& ticketNumber, const std::string& passengerId,
           const std::string& flightNumber, TicketClass ticketClass,
           double price, int freeBaggageAllowance);
    virtual ~Ticket();
    
    std::string getTicketNumber() const { return ticketNumber; }
    std::string getPassengerId() const { return passengerId; }
    std::string getFlightNumber() const { return flightNumber; }
    TicketClass getTicketClass() const { return ticketClass; }
    std::chrono::system_clock::time_point getIssueDate() const { return issueDate; }
    bool getCheckedIn() const { return checkedIn; }
    CheckInType getCheckInType() const { return checkInType; }
    std::chrono::system_clock::time_point getCheckInTime() const { return checkInTime; }
    bool getBoarded() const { return boarded; }
    int getSeatNumber() const { return seatNumber; }
    double getPrice() const { return price; }
    int getFreeBaggageAllowance() const { return freeBaggageAllowance; }
    
    void checkIn(CheckInType type);
    void board();
    void assignSeat(int seatNumber);
    
    virtual void displayInfo() const;
    virtual std::string getClassString() const;
    virtual std::string toJson() const;
    static Ticket* fromJson(const std::string& json);
};

class EconomyTicket : public Ticket {
public:
    EconomyTicket(const std::string& ticketNumber, const std::string& passengerId,
                  const std::string& flightNumber, double price);
    ~EconomyTicket() override;
    
    std::string toJson() const override;
    static EconomyTicket* fromJson(const std::string& json);
};

class BusinessTicket : public Ticket {
public:
    BusinessTicket(const std::string& ticketNumber, const std::string& passengerId,
                   const std::string& flightNumber, double price);
    ~BusinessTicket() override;
    
    std::string toJson() const override;
    static BusinessTicket* fromJson(const std::string& json);
};

class FirstClassTicket : public Ticket {
public:
    FirstClassTicket(const std::string& ticketNumber, const std::string& passengerId,
                     const std::string& flightNumber, double price);
    ~FirstClassTicket() override;
    
    std::string toJson() const override;
    static FirstClassTicket* fromJson(const std::string& json);
};

#endif
