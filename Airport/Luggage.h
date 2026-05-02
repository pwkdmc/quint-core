#ifndef AIRPORT_LUGGAGE_H
#define AIRPORT_LUGGAGE_H

#include <string>
#include <chrono>
#include "exceptions.h"

enum class LuggageStatus {
    CheckedIn,
    InTransit,
    OnBoard,
    Delivered,
    Lost
};

class Luggage {
protected:
    std::string luggageTag;
    std::string passengerId;
    std::string ticketNumber;
    double weight;
    double actualWeight;
    LuggageStatus status;
    std::chrono::system_clock::time_point checkInTime;
    double overweightFee;

public:
    Luggage(const std::string& luggageTag, const std::string& passengerId,
            const std::string& ticketNumber, double weight);
    virtual ~Luggage();
    
    std::string getLuggageTag() const { return luggageTag; }
    std::string getPassengerId() const { return passengerId; }
    std::string getTicketNumber() const { return ticketNumber; }
    double getWeight() const { return weight; }
    double getActualWeight() const { return actualWeight; }
    LuggageStatus getStatus() const { return status; }
    std::chrono::system_clock::time_point getCheckInTime() const { return checkInTime; }
    double getOverweightFee() const { return overweightFee; }
    
    void setActualWeight(double weight);
    void setStatus(LuggageStatus status);
    double calculateOverweightFee(double freeAllowance, double overweightRate) const;
    
    virtual void displayInfo() const;
    virtual std::string getStatusString() const;
    virtual std::string toJson() const;
    static Luggage* fromJson(const std::string& json);
};

class CarryOnLuggage : public Luggage {
private:
    bool isCarried;

public:
    CarryOnLuggage(const std::string& luggageTag, const std::string& passengerId,
                   const std::string& ticketNumber, double weight);
    ~CarryOnLuggage() override;
    
    bool getIsCarried() const { return isCarried; }
    void setCarried(bool carried);
    
    void displayInfo() const override;
    std::string toJson() const override;
    static CarryOnLuggage* fromJson(const std::string& json);
};

class CheckedLuggage : public Luggage {
private:
    std::string currentLocation;

public:
    CheckedLuggage(const std::string& luggageTag, const std::string& passengerId,
                   const std::string& ticketNumber, double weight);
    ~CheckedLuggage() override;
    
    std::string getCurrentLocation() const { return currentLocation; }
    void setCurrentLocation(const std::string& location);
    
    void displayInfo() const override;
    std::string toJson() const override;
    static CheckedLuggage* fromJson(const std::string& json);
};

#endif
