#ifndef AIRPORT_PASSENGER_H
#define AIRPORT_PASSENGER_H

#include <string>
#include <vector>
#include <chrono>
#include "exceptions.h"

enum class PassengerType {
    Economy,
    Business,
    VIP
};

struct TicketInfo {
    std::string ticketNumber;
    std::string flightNumber;
    bool checkedIn;
    bool boarded;
};

struct LuggageInfo {
    std::string luggageTag;
    double weight;
    bool checked;
};

class Passenger {
protected:
    std::string id;
    std::string firstName;
    std::string lastName;
    std::string passportNumber;
    int age;
    PassengerType type;
    std::vector<std::string> ticketNumbers;
    std::vector<std::string> luggageTags;
    int priorityLevel;

public:
    Passenger(const std::string& id, const std::string& firstName, 
              const std::string& lastName, const std::string& passportNumber,
              int age, PassengerType type);
    virtual ~Passenger();
    
    std::string getId() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getFullName() const { return firstName + " " + lastName; }
    std::string getPassportNumber() const { return passportNumber; }
    int getAge() const { return age; }
    PassengerType getType() const { return type; }
    const std::vector<std::string>& getTicketNumbers() const { return ticketNumbers; }
    const std::vector<std::string>& getLuggageTags() const { return luggageTags; }
    int getPriorityLevel() const { return priorityLevel; }
    
    void addTicket(const std::string& ticketNumber);
    void addLuggage(const std::string& luggageTag);
    void removeTicket(const std::string& ticketNumber);
    void removeLuggage(const std::string& luggageTag);
    
    virtual void displayInfo() const;
    virtual std::string getTypeString() const;
    virtual std::string toJson() const;
    static Passenger* fromJson(const std::string& json);
    
    virtual int getFreeBaggageAllowance() const;
    virtual double getBaggageOverweightRate() const;
};

class EconomyPassenger : public Passenger {
public:
    EconomyPassenger(const std::string& id, const std::string& firstName,
                     const std::string& lastName, const std::string& passportNumber, int age);
    ~EconomyPassenger() override;
    
    std::string getTypeString() const override;
    int getFreeBaggageAllowance() const override;
    double getBaggageOverweightRate() const override;
    std::string toJson() const override;
    static EconomyPassenger* fromJson(const std::string& json);
};

class BusinessPassenger : public Passenger {
public:
    BusinessPassenger(const std::string& id, const std::string& firstName,
                      const std::string& lastName, const std::string& passportNumber, int age);
    ~BusinessPassenger() override;
    
    std::string getTypeString() const override;
    int getFreeBaggageAllowance() const override;
    double getBaggageOverweightRate() const override;
    std::string toJson() const override;
    static BusinessPassenger* fromJson(const std::string& json);
};

class VIPPassenger : public Passenger {
public:
    VIPPassenger(const std::string& id, const std::string& firstName,
                 const std::string& lastName, const std::string& passportNumber, int age);
    ~VIPPassenger() override;
    
    std::string getTypeString() const override;
    int getFreeBaggageAllowance() const override;
    double getBaggageOverweightRate() const override;
    std::string toJson() const override;
    static VIPPassenger* fromJson(const std::string& json);
};

#endif
