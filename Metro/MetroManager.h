#ifndef METRO_MANAGER_H
#define METRO_MANAGER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <chrono>

class Station;
class Line;
class Transfer;
class EmergencySituation;

struct RouteStep {
    std::string fromStation;
    std::string toStation;
    std::string fromStationLine;
    std::string toStationLine;
    std::string lineName;
    std::string lineColor;
    double travelTimeMinutes;
    bool isTransfer;
    double transferTimeMinutes;
    int arrivalHour;
    int arrivalMinute;
    double intervalMinutes;
};

struct CalculatedRoute {
    std::vector<RouteStep> steps;
    double totalTimeMinutes;
    std::string description;
};

class Station {
private:
    std::string name;
    std::vector<Line*> connectedLines;
    std::map<std::string, double> transfers;
    bool isClosed;
    
public:
    explicit Station(const std::string& name);
    ~Station();
    
    std::string getName() const { return name; }
    const std::vector<Line*>& getLines() const { return connectedLines; }
    const std::map<std::string, double>& getTransfers() const { return transfers; }
    bool getIsClosed() const { return isClosed; }
    void setIsClosed(bool closed) { isClosed = closed; }
    
    void addLine(Line* line);
    void addTransfer(const std::string& stationName, double timeMinutes);
    double getTransferTimeTo(const std::string& stationName) const;
    
    void displayInfo(const std::vector<Station*>& allStations) const;
};

class Line {
private:
    std::string name;
    std::string color;
    std::vector<Station*> stations;
    std::map<std::string, std::map<std::string, double>> travelTimes;
    double baseIntervalPeak;
    double baseIntervalNight;
    double currentDelay;
    bool isClosed;
    
public:
    Line(const std::string& name, const std::string& color, 
         double peakInterval = 3.0, double nightInterval = 10.0);
    ~Line();
    
    std::string getName() const { return name; }
    std::string getColor() const { return color; }
    const std::vector<Station*>& getStations() const { return stations; }
    double getCurrentDelay() const { return currentDelay; }
    bool getIsClosed() const { return isClosed; }
    
    void addStation(Station* station);
    void setTravelTime(Station* from, Station* to, double timeMinutes);
    double getTravelTime(Station* from, Station* to) const;
    void addDelay(double delayMinutes);
    void removeDelay();
    void setIsClosed(bool closed) { isClosed = closed; }
    
    double getIntervalAtTime(int hour) const;
    void displayInfo() const;
    void displaySchedule() const;
};

class Transfer {
private:
    Station* fromStation;
    Station* toStation;
    double timeMinutes;
    
public:
    Transfer(Station* from, Station* to, double timeMinutes);
    ~Transfer();
    
    Station* getFromStation() const { return fromStation; }
    Station* getToStation() const { return toStation; }
    double getTimeMinutes() const { return timeMinutes; }
};

class EmergencySituation {
private:
    int id;
    std::string type;
    std::string lineName;
    std::string stationName;
    double delayMinutes;
    
public:
    EmergencySituation(int id, const std::string& type, 
                       const std::string& lineOrStation, double delay = 0);
    ~EmergencySituation();
    
    int getId() const { return id; }
    std::string getType() const { return type; }
    std::string getLineName() const { return lineName; }
    std::string getStationName() const { return stationName; }
    double getDelayMinutes() const { return delayMinutes; }
    
    void apply() const;
    void displayInfo() const;
};

class MetroManager {
private:
    std::vector<Line*> lines;
    std::vector<Station*> stations;
    std::vector<Transfer*> transfers;
    std::vector<EmergencySituation*> emergencySituations;
    std::string dataFile;
    int nextEmergencyId;
    
    void initializeMetroNetwork();
    void displayLinesMenu();
    void displayStationsMenu();
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getValidatedString(const std::string& prompt);
    int getValidatedHour(const std::string& prompt);
    Line* findLineByName(const std::string& name) const;
    Station* findStationByName(const std::string& name) const;
    CalculatedRoute findShortestRoute(Station* from, Station* to, int departureHour) const;
    double calculateTravelTime(Station* from, Station* to, int hour) const;
    bool saveToFile() const;
    bool loadFromFile();
    
public:
    MetroManager();
    ~MetroManager();
    
    int run(int lastAction = 0);
    
private:
    void viewLine();
    void viewSchedule();
    void calculateRoute();
    void addEmergency();
    void removeEmergency();
};

#endif
