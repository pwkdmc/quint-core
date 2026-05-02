#include <iostream>
#include <limits>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <algorithm>
#include "MetroManager.h"
#include "exceptions.h"

Station::Station(const std::string& name) 
    : name(name), isClosed(false) {
    std::cout << "\033[1;31mСоздан Station\033[0m\n";
}

Station::~Station() {
    std::cout << "\033[1;31mУдален Station\033[0m\n";
}

void Station::addLine(Line* line) {
    connectedLines.push_back(line);
}

void Station::addTransfer(const std::string& stationName, double timeMinutes) {
    transfers[stationName] = timeMinutes;
}

double Station::getTransferTimeTo(const std::string& stationName) const {
    auto it = transfers.find(stationName);
    if (it != transfers.end()) {
        return it->second;
    }
    return 0;
}

void Station::displayInfo(const std::vector<Station*>& allStations) const {
    std::cout << "Станция: " << name << "\n";
    std::cout << "Линии: ";
    for (size_t i = 0; i < connectedLines.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << connectedLines[i]->getName() << " (" << connectedLines[i]->getColor() << ")";
    }
    std::cout << "\n";
    
    if (!transfers.empty()) {
        std::cout << "Пересадки:\n";
        for (const auto& transfer : transfers) {
            Station* transferStation = nullptr;
            for (size_t i = 0; i < allStations.size(); ++i) {
                if (allStations[i]->getName() == transfer.first) {
                    transferStation = allStations[i];
                    break;
                }
            }
            if (transferStation) {
                bool hasCommonLine = false;
                for (size_t i = 0; i < connectedLines.size(); ++i) {
                    for (size_t j = 0; j < transferStation->getLines().size(); ++j) {
                        if (connectedLines[i]->getName() == transferStation->getLines()[j]->getName()) {
                            hasCommonLine = true;
                            break;
                        }
                    }
                    if (hasCommonLine) break;
                }
                
                if (!hasCommonLine) {
                    std::cout << "- " << transfer.first << " (линии: ";
                    const auto& lines = transferStation->getLines();
                    bool first = true;
                    for (size_t j = 0; j < lines.size(); ++j) {
                        if (!first) std::cout << ", ";
                        std::cout << lines[j]->getName() << " (" << lines[j]->getColor() << ")";
                        first = false;
                    }
                    std::cout << "): " << transfer.second << " мин\n";
                }
            }
        }
    }
    
    if (isClosed) {
        std::cout << "Станция закрыта\n";
    }
}

Line::Line(const std::string& name, const std::string& color, 
           double peakInterval, double nightInterval)
    : name(name), color(color), baseIntervalPeak(peakInterval), 
      baseIntervalNight(nightInterval), currentDelay(0), isClosed(false) {
    std::cout << "\033[1;31mСоздан Line\033[0m\n";
}

Line::~Line() {
    std::cout << "\033[1;31mУдален Line\033[0m\n";
}

void Line::addStation(Station* station) {
    stations.push_back(station);
    station->addLine(this);
}

void Line::setTravelTime(Station* from, Station* to, double timeMinutes) {
    travelTimes[from->getName()][to->getName()] = timeMinutes;
    travelTimes[to->getName()][from->getName()] = timeMinutes;
}

double Line::getTravelTime(Station* from, Station* to) const {
    auto fromIt = travelTimes.find(from->getName());
    if (fromIt != travelTimes.end()) {
        auto toIt = fromIt->second.find(to->getName());
        if (toIt != fromIt->second.end()) {
            return toIt->second;
        }
    }
    
    int fromPos = -1, toPos = -1;
    for (size_t i = 0; i < stations.size(); ++i) {
        if (stations[i] == from) fromPos = (int)i;
        if (stations[i] == to) toPos = (int)i;
    }
    
    if (fromPos < 0 || toPos < 0) return 0;
    if (fromPos == toPos) return 0;
    
    int start = std::min(fromPos, toPos);
    int end = std::max(fromPos, toPos);
    
    double totalTime = 0;
    for (int i = start; i < end; ++i) {
        double segmentTime = getTravelTime(stations[i], stations[i+1]);
        if (segmentTime <= 0) return 0;
        totalTime += segmentTime;
    }
    
    return totalTime;
}

void Line::addDelay(double delayMinutes) {
    currentDelay += delayMinutes;
}

void Line::removeDelay() {
    currentDelay = 0;
}

double Line::getIntervalAtTime(int hour) const {
    if (isClosed) {
        return -1;
    }
    
    bool isPeak = (hour >= 7 && hour <= 9) || (hour >= 17 && hour <= 20);
    double baseInterval = isPeak ? baseIntervalPeak : baseIntervalNight;
    return baseInterval + currentDelay;
}

void Line::displayInfo() const {
    std::cout << "Линия: " << name << " (" << color << ")\n";
    std::cout << "Станции:";
    for (size_t i = 0; i < stations.size(); ++i) {
        std::cout << i + 1 << ". " << stations[i]->getName() << "\n";
    }
    if (currentDelay > 0) {
        std::cout << "Задержка: " << currentDelay << " мин\n";
    }
    if (isClosed) {
        std::cout << "Линия закрыта\n";
    }
}

void Line::displaySchedule() const {
    std::cout << "Расписание движения по линии " << name << ":\n";
    std::cout << "Интервалы движения:\n";
    
    std::cout << "00:00 - 06:59: " << std::fixed << std::setprecision(1) 
              << baseIntervalNight << " мин\n";
    std::cout << "07:00 - 09:59: " << std::fixed << std::setprecision(1) 
              << baseIntervalPeak << " мин (час пик)\n";
    std::cout << "10:00 - 16:59: " << std::fixed << std::setprecision(1) 
              << baseIntervalNight << " мин\n";
    std::cout << "17:00 - 20:59: " << std::fixed << std::setprecision(1) 
              << baseIntervalPeak << " мин (час пик)\n";
    std::cout << "21:00 - 23:59: " << std::fixed << std::setprecision(1) 
              << baseIntervalNight << " мин\n";
    
    if (currentDelay > 0) {
        std::cout << "Текущая задержка: " << currentDelay << " мин\n";
    }
    if (isClosed) {
        std::cout << "Линия закрыта\n";
    }
}

Transfer::Transfer(Station* from, Station* to, double timeMinutes)
    : fromStation(from), toStation(to), timeMinutes(timeMinutes) {
    std::cout << "\033[1;31mСоздан Transfer\033[0m\n";
}

Transfer::~Transfer() {
    std::cout << "\033[1;31mУдален Transfer\033[0m\n";
}

EmergencySituation::EmergencySituation(int id, const std::string& type, 
                                       const std::string& lineOrStation, double delay)
    : id(id), type(type), lineName(type == "delay" ? lineOrStation : ""),
      stationName(type == "closure" ? lineOrStation : ""), delayMinutes(delay) {
    std::cout << "\033[1;31mСоздана EmergencySituation\033[0m\n";
}

EmergencySituation::~EmergencySituation() {
    std::cout << "\033[1;31mУдалена EmergencySituation\033[0m\n";
}

void EmergencySituation::displayInfo() const {
    std::cout << "Аварийная ситуация #" << id << ":\n";
    if (type == "delay") {
        std::cout << "Тип: Задержка движения\n";
        std::cout << "Линия: " << lineName << "\n";
        std::cout << "Задержка: " << delayMinutes << " мин\n";
    } else if (type == "closure") {
        std::cout << "Тип: Закрытие станции\n";
        std::cout << "Станция: " << stationName << "\n";
    }
}

MetroManager::MetroManager() : dataFile("metro_data.txt"), nextEmergencyId(1) {
    std::cout << "\033[1;31mСоздан MetroManager\033[0m\n";
    initializeMetroNetwork();
    loadFromFile();
    saveToFile();
}
    
MetroManager::~MetroManager() {
    saveToFile();
    
    for (size_t i = 0; i < lines.size(); ++i) {
        delete lines[i];
    }
    lines.clear();
    
    for (size_t i = 0; i < stations.size(); ++i) {
        delete stations[i];
    }
    stations.clear();
    
    for (size_t i = 0; i < transfers.size(); ++i) {
        delete transfers[i];
    }
    transfers.clear();
    
    for (size_t i = 0; i < emergencySituations.size(); ++i) {
        delete emergencySituations[i];
    }
    emergencySituations.clear();
    
    std::cout << "\033[1;31mУдален MetroManager\033[0m\n";
}

void MetroManager::initializeMetroNetwork() {
    Line* sokolnicheskaya_line = new Line("Сокольническая", "Красный", 2.0, 8.0);
    Line* zamoskvoretskaya_line = new Line("Замоскворецкая", "Зеленый", 2.5, 9.0);
    Line* arbatsko_pokrovskaya_line = new Line("Арбатско-Покровская", "Синий", 3.0, 10.0);
    Line* filevskaya_line = new Line("Филевская", "Голубой", 4.0, 12.0);
    Line* kolcevaya_line = new Line("Кольцевая", "Коричневый", 3.0, 10.0);
    lines.push_back(sokolnicheskaya_line);
    lines.push_back(zamoskvoretskaya_line);
    lines.push_back(arbatsko_pokrovskaya_line);
    lines.push_back(filevskaya_line);
    lines.push_back(kolcevaya_line);

    Station* smoelnaya = new Station("Смоленская");
    stations.push_back(smoelnaya);
    Station* park_kultury = new Station("Парк культуры");
    stations.push_back(park_kultury);
    Station* oktyabrskaya = new Station("Октябрьская");
    stations.push_back(oktyabrskaya);
    Station* taganskaya = new Station("Таганская");
    stations.push_back(taganskaya);
    Station* komsomolskaya = new Station("Комсомольская");
    stations.push_back(komsomolskaya);
    Station* belpost = new Station("Белорусская");
    stations.push_back(belpost);
    Station* novokuznetskaya = new Station("Новокузнецкая");
    stations.push_back(novokuznetskaya);
    Station* tretiakovskaya = new Station("Третьяковская");
    stations.push_back(tretiakovskaya);
    Station* aviamotornaya = new Station("Авиамоторная");
    stations.push_back(aviamotornaya);
    Station* leninskaya = new Station("Ленинская");
    stations.push_back(leninskaya);
    Station* kiyevskaya = new Station("Киевская");
    stations.push_back(kiyevskaya);
    Station* krasnye_vorota = new Station("Красные ворота");
    stations.push_back(krasnye_vorota);
    Station* izmailovskaya = new Station("Измайловская");
    stations.push_back(izmailovskaya);
    Station* partiyskaya = new Station("Партизанская");
    stations.push_back(partiyskaya);
    Station* oktyabrskaya_ring = new Station("Октябрьская кольцевая");
    stations.push_back(oktyabrskaya_ring);
    Station* park_kultury_ring = new Station("Парк культуры кольцевая");
    stations.push_back(park_kultury_ring);
    Station* kiyevskaya_ring = new Station("Киевская кольцевая");
    stations.push_back(kiyevskaya_ring);
    Station* belpost_ring = new Station("Белорусская кольцевая");
    stations.push_back(belpost_ring);
    Station* novokuznetskaya_ring = new Station("Новокузнецкая кольцевая");
    stations.push_back(novokuznetskaya_ring);
    Station* tretiakovskaya_ring = new Station("Третьяковская кольцевая");
    stations.push_back(tretiakovskaya_ring);
    Station* taganskaya_ring = new Station("Таганская кольцевая");
    stations.push_back(taganskaya_ring);
    Station* komsomolskaya_ring = new Station("Комсомольская кольцевая");
    stations.push_back(komsomolskaya_ring);

    sokolnicheskaya_line->addStation(smoelnaya);
    sokolnicheskaya_line->addStation(park_kultury);
    sokolnicheskaya_line->addStation(oktyabrskaya);
    sokolnicheskaya_line->addStation(taganskaya);
    sokolnicheskaya_line->addStation(komsomolskaya);
    
    zamoskvoretskaya_line->addStation(belpost);
    zamoskvoretskaya_line->addStation(novokuznetskaya);
    zamoskvoretskaya_line->addStation(tretiakovskaya);
    zamoskvoretskaya_line->addStation(aviamotornaya);
    zamoskvoretskaya_line->addStation(leninskaya);
    
    arbatsko_pokrovskaya_line->addStation(smoelnaya);
    arbatsko_pokrovskaya_line->addStation(kiyevskaya);
    arbatsko_pokrovskaya_line->addStation(park_kultury);
    arbatsko_pokrovskaya_line->addStation(komsomolskaya);
    arbatsko_pokrovskaya_line->addStation(krasnye_vorota);
    arbatsko_pokrovskaya_line->addStation(izmailovskaya);
    arbatsko_pokrovskaya_line->addStation(partiyskaya);
    
    filevskaya_line->addStation(smoelnaya);
    filevskaya_line->addStation(kiyevskaya);
    
    kolcevaya_line->addStation(kiyevskaya_ring);
    kolcevaya_line->addStation(belpost_ring);
    kolcevaya_line->addStation(novokuznetskaya_ring);
    kolcevaya_line->addStation(tretiakovskaya_ring);
    kolcevaya_line->addStation(taganskaya_ring);
    kolcevaya_line->addStation(komsomolskaya_ring);
    kolcevaya_line->addStation(oktyabrskaya_ring);
    kolcevaya_line->addStation(park_kultury_ring);
    
    park_kultury->addTransfer("Парк культуры кольцевая", 4.0);
    park_kultury_ring->addTransfer("Парк культуры", 4.0);
    oktyabrskaya->addTransfer("Октябрьская кольцевая", 3.0);
    oktyabrskaya_ring->addTransfer("Октябрьская", 3.0);
    taganskaya->addTransfer("Таганская кольцевая", 4.0);
    taganskaya_ring->addTransfer("Таганская", 4.0);
    komsomolskaya->addTransfer("Комсомольская кольцевая", 5.0);
    komsomolskaya_ring->addTransfer("Комсомольская", 5.0);
    belpost->addTransfer("Белорусская кольцевая", 4.0);
    belpost_ring->addTransfer("Белорусская", 4.0);
    novokuznetskaya->addTransfer("Новокузнецкая кольцевая", 3.0);
    novokuznetskaya_ring->addTransfer("Новокузнецкая", 3.0);
    tretiakovskaya->addTransfer("Третьяковская кольцевая", 3.0);
    tretiakovskaya_ring->addTransfer("Третьяковская", 3.0);

    sokolnicheskaya_line->setTravelTime(smoelnaya, park_kultury, 5.0);
    sokolnicheskaya_line->setTravelTime(park_kultury, oktyabrskaya, 4.0);
    sokolnicheskaya_line->setTravelTime(oktyabrskaya, taganskaya, 6.0);
    sokolnicheskaya_line->setTravelTime(taganskaya, komsomolskaya, 5.0);
    zamoskvoretskaya_line->setTravelTime(belpost, novokuznetskaya, 4.0);
    zamoskvoretskaya_line->setTravelTime(novokuznetskaya, tretiakovskaya, 3.0);
    zamoskvoretskaya_line->setTravelTime(tretiakovskaya, aviamotornaya, 5.0);
    zamoskvoretskaya_line->setTravelTime(aviamotornaya, leninskaya, 6.0);
    arbatsko_pokrovskaya_line->setTravelTime(smoelnaya, kiyevskaya, 3.0);
    arbatsko_pokrovskaya_line->setTravelTime(kiyevskaya, park_kultury, 4.0);
    arbatsko_pokrovskaya_line->setTravelTime(park_kultury, komsomolskaya, 8.0);
    arbatsko_pokrovskaya_line->setTravelTime(komsomolskaya, krasnye_vorota, 3.0);
    arbatsko_pokrovskaya_line->setTravelTime(krasnye_vorota, izmailovskaya, 4.0);
    arbatsko_pokrovskaya_line->setTravelTime(izmailovskaya, partiyskaya, 3.0);
    filevskaya_line->setTravelTime(smoelnaya, kiyevskaya, 3.0);
    kolcevaya_line->setTravelTime(kiyevskaya_ring, belpost_ring, 5.0);
    kolcevaya_line->setTravelTime(belpost_ring, novokuznetskaya_ring, 4.0);
    kolcevaya_line->setTravelTime(novokuznetskaya_ring, tretiakovskaya_ring, 3.0);
    kolcevaya_line->setTravelTime(tretiakovskaya_ring, taganskaya_ring, 4.0);
    kolcevaya_line->setTravelTime(taganskaya_ring, komsomolskaya_ring, 5.0);
    kolcevaya_line->setTravelTime(komsomolskaya_ring, oktyabrskaya_ring, 6.0);
    kolcevaya_line->setTravelTime(oktyabrskaya_ring, park_kultury_ring, 5.0);
    kolcevaya_line->setTravelTime(park_kultury_ring, kiyevskaya_ring, 4.0);
    
    transfers.push_back(new Transfer(park_kultury, park_kultury_ring, 4.0));
    transfers.push_back(new Transfer(park_kultury_ring, park_kultury, 4.0));
    transfers.push_back(new Transfer(oktyabrskaya, oktyabrskaya_ring, 3.0));
    transfers.push_back(new Transfer(oktyabrskaya_ring, oktyabrskaya, 3.0));
    transfers.push_back(new Transfer(taganskaya, taganskaya_ring, 4.0));
    transfers.push_back(new Transfer(taganskaya_ring, taganskaya, 4.0));
    transfers.push_back(new Transfer(komsomolskaya, komsomolskaya_ring, 5.0));
    transfers.push_back(new Transfer(komsomolskaya_ring, komsomolskaya, 5.0));
    transfers.push_back(new Transfer(belpost, belpost_ring, 4.0));
    transfers.push_back(new Transfer(belpost_ring, belpost, 4.0));
    transfers.push_back(new Transfer(novokuznetskaya, novokuznetskaya_ring, 3.0));
    transfers.push_back(new Transfer(novokuznetskaya_ring, novokuznetskaya, 3.0));
    transfers.push_back(new Transfer(tretiakovskaya, tretiakovskaya_ring, 3.0));
    transfers.push_back(new Transfer(tretiakovskaya_ring, tretiakovskaya, 3.0));
}

bool MetroManager::saveToFile() const {
    std::ofstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "EMERGENCIES\n";
    for (size_t i = 0; i < emergencySituations.size(); ++i) {
        file << emergencySituations[i]->getId() << "|"
             << emergencySituations[i]->getType() << "|"
             << emergencySituations[i]->getLineName() << "|"
             << emergencySituations[i]->getStationName() << "|"
             << emergencySituations[i]->getDelayMinutes() << "\n";
    }
    
    file.close();
    return true;
}

bool MetroManager::loadFromFile() {
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string section = "";
    
    while (std::getline(file, line)) {
        if (line == "EMERGENCIES") {
            section = line;
            continue;
        }
        
        if (section == "EMERGENCIES" && !line.empty()) {
            std::istringstream iss(line);
            std::string id_str, type, lineName, stationName, delay_str;
            
            std::getline(iss, id_str, '|');
            std::getline(iss, type, '|');
            std::getline(iss, lineName, '|');
            std::getline(iss, stationName, '|');
            std::getline(iss, delay_str, '|');
            
            int id = std::stoi(id_str);
            double delay = std::stod(delay_str);
            
            if (id >= nextEmergencyId) {
                nextEmergencyId = id + 1;
            }
            
            if (type == "delay" && !lineName.empty()) {
                Line* targetLine = findLineByName(lineName);
                if (targetLine) {
                    targetLine->addDelay(delay);
                }
                emergencySituations.push_back(new EmergencySituation(id, "delay", lineName, delay));
            } else if (type == "closure" && !stationName.empty()) {
                Station* targetStation = findStationByName(stationName);
                if (targetStation) {
                    targetStation->setIsClosed(true);
                }
                emergencySituations.push_back(new EmergencySituation(id, "closure", stationName, 0));
            }
        }
    }
    
    file.close();
    return true;
}

int MetroManager::getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    std::cout << prompt;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw MetroInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (value < min || value > max) {
        throw MetroInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    
    return value;
}

std::string MetroManager::getValidatedString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty()) {
        throw MetroInputException("Поле не может быть пустым");
    }
    return value;
}

int MetroManager::getValidatedHour(const std::string& prompt) {
    int hour;
    std::cout << prompt;
    if (!(std::cin >> hour)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw MetroInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (hour < 0 || hour > 23) {
        throw MetroInputException("Час должен быть от 0 до 23");
    }
    
    return hour;
}

Line* MetroManager::findLineByName(const std::string& name) const {
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i]->getName() == name) {
            return lines[i];
        }
    }
    return nullptr;
}

Station* MetroManager::findStationByName(const std::string& name) const {
    for (size_t i = 0; i < stations.size(); ++i) {
        if (stations[i]->getName() == name) {
            return stations[i];
        }
    }
    return nullptr;
}

double MetroManager::calculateTravelTime(Station* from, Station* to, int hour) const {
    for (size_t i = 0; i < lines.size(); ++i) {
        const std::vector<Station*>& lineStations = lines[i]->getStations();
        for (size_t j = 0; j < lineStations.size(); ++j) {
            if (lineStations[j] == from) {
                for (size_t k = 0; k < lineStations.size(); ++k) {
                    if (lineStations[k] == to) {
                        double time = lines[i]->getTravelTime(from, to);
                        if (time > 0) {
                            double interval = lines[i]->getIntervalAtTime(hour);
                            if (interval < 0) {
                                return -1;
                            }
                            return time + lines[i]->getCurrentDelay();
                        }
                    }
                }
            }
        }
    }
    return -1;
}

CalculatedRoute MetroManager::findShortestRoute(Station* from, Station* to, int departureHour) const {
    CalculatedRoute result;
    result.totalTimeMinutes = -1;
    
    std::map<std::string, double> dist;
    std::map<std::string, std::string> prev;
    std::map<std::string, std::string> prevLine;
    std::map<std::string, double> prevTime;
    std::set<std::string> visited;
    std::priority_queue<std::pair<double, std::string>, 
                        std::vector<std::pair<double, std::string>>,
                        std::greater<std::pair<double, std::string>>> pq;
    
    for (size_t i = 0; i < stations.size(); ++i) {
        dist[stations[i]->getName()] = 1e9;
    }
    dist[from->getName()] = 0;
    prevTime[from->getName()] = departureHour * 60;
    pq.push({0, from->getName()});
    
    while (!pq.empty()) {
        std::string u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        
        if (visited.count(u)) continue;
        visited.insert(u);
        
        Station* uStation = findStationByName(u);
        if (!uStation) continue;
        
        if (u == to->getName()) {
            if (!uStation->getIsClosed()) break;
        }
        
        for (size_t i = 0; i < lines.size(); ++i) {
            const std::vector<Station*>& lineStations = lines[i]->getStations();
            
            int uPos = -1;
            for (size_t j = 0; j < lineStations.size(); ++j) {
                if (lineStations[j]->getName() == u) {
                    uPos = (int)j;
                    break;
                }
            }
            
            if (uPos < 0) continue;
            
            for (size_t k = 0; k < lineStations.size(); ++k) {
                if (k == (size_t)uPos) continue;
                
                std::string v = lineStations[k]->getName();
                if (visited.count(v)) continue;
                
                if (uStation->getIsClosed()) continue;
                
                double travelTime = lines[i]->getTravelTime(uStation, lineStations[k]);
                if (travelTime <= 0) continue;
                
                double interval = lines[i]->getIntervalAtTime(departureHour+dist[u]);
                if (interval < 0) continue;
                
                double weight = travelTime + interval + lines[i]->getCurrentDelay();
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    prevLine[v] = lines[i]->getName();
                    prevTime[v] = prevTime[u] + travelTime + interval;
                    pq.push({dist[v], v});
                }
            }
        }
        
        const auto& transfers = uStation->getTransfers();
        for (const auto& transfer : transfers) {
            std::string v = transfer.first;
            if (visited.count(v)) continue;
            if (v == u) continue;
            
            if (uStation->getIsClosed()) continue;
            
            Station* vStation = findStationByName(v);
            if (!vStation) continue;
            
            if (vStation->getIsClosed()) continue;
            
            double transferTime = transfer.second;
            if (dist[u] + transferTime < dist[v]) {
                dist[v] = dist[u] + transferTime;
                prev[v] = u;
                prevLine[v] = "transfer";
                prevTime[v] = prevTime[u] + transferTime;
                pq.push({dist[v], v});
            }
        }
    }
    
    if (dist[to->getName()] >= 1e9 || to->getIsClosed()) {
        return result;
    }
    
    result.totalTimeMinutes = dist[to->getName()];
    
    std::vector<std::string> path;
    std::vector<std::string> pathLines;
    std::string current = to->getName();
    while (current != from->getName()) {
        path.push_back(current);
        pathLines.push_back(prevLine[current]);
        current = prev[current];
    }
    path.push_back(from->getName());
    pathLines.push_back("");
    std::reverse(path.begin(), path.end());
    std::reverse(pathLines.begin(), pathLines.end());
    
    int currentTime = departureHour * 60;
    
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        RouteStep step;
        step.fromStation = path[i];
        step.toStation = path[i + 1];
        step.lineName = pathLines[i + 1];
        
        Station* fromSt = findStationByName(step.fromStation);
        Station* toSt = findStationByName(step.toStation);
        
        if (step.lineName == "transfer") {
            if (fromSt && !fromSt->getLines().empty()) {
                for (size_t j = 0; j < fromSt->getLines().size(); ++j) {
                    bool foundOnNext = false;
                    if (toSt) {
                        for (size_t k = 0; k < toSt->getLines().size(); ++k) {
                            if (fromSt->getLines()[j]->getName() == toSt->getLines()[k]->getName()) {
                                foundOnNext = true;
                                break;
                            }
                        }
                    }
                    if (!foundOnNext) {
                        step.fromStationLine = fromSt->getLines()[j]->getName();
                        break;
                    }
                }
                if (step.fromStationLine.empty() && !fromSt->getLines().empty()) {
                    step.fromStationLine = fromSt->getLines()[0]->getName();
                }
            }
            if (toSt && !toSt->getLines().empty()) {
                for (size_t j = 0; j < toSt->getLines().size(); ++j) {
                    bool foundOnPrev = false;
                    if (fromSt) {
                        for (size_t k = 0; k < fromSt->getLines().size(); ++k) {
                            if (toSt->getLines()[j]->getName() == fromSt->getLines()[k]->getName()) {
                                foundOnPrev = true;
                                break;
                            }
                        }
                    }
                    if (!foundOnPrev) {
                        step.toStationLine = toSt->getLines()[j]->getName();
                        break;
                    }
                }
                if (step.toStationLine.empty() && !toSt->getLines().empty()) {
                    step.toStationLine = toSt->getLines()[0]->getName();
                }
            }
        } else {
            step.fromStationLine = step.lineName;
            step.toStationLine = step.lineName;
        }
        
        bool isTransferStep = false;
        if (step.lineName == "transfer" || step.lineName.empty()) {
            isTransferStep = true;
        } else {
            Line* line = findLineByName(step.lineName);
            if (line) {
                double directTime = line->getTravelTime(fromSt, toSt);
                if (directTime <= 0) {
                    isTransferStep = true;
                }
            } else {
                isTransferStep = true;
            }
        }
        
        if (isTransferStep) {
            step.isTransfer = true;
            step.transferTimeMinutes = fromSt ? fromSt->getTransferTimeTo(step.toStation) : 0;
            step.lineName = "";
            step.lineColor = "";
            step.travelTimeMinutes = 0;
            step.arrivalHour = (currentTime + step.transferTimeMinutes) / 60;
            step.arrivalMinute = (int)(currentTime + step.transferTimeMinutes) % 60;
            step.intervalMinutes = 0;
            currentTime += step.transferTimeMinutes;
        } else {
            step.isTransfer = false;
            Line* line = findLineByName(step.lineName);
            if (line) {
                step.lineColor = line->getColor();
                step.travelTimeMinutes = line->getTravelTime(fromSt, toSt);
                step.intervalMinutes = line->getIntervalAtTime(currentTime / 60);
                step.arrivalHour = (currentTime + step.travelTimeMinutes + step.intervalMinutes) / 60;
                step.arrivalMinute = (int)(currentTime + step.travelTimeMinutes + step.intervalMinutes) % 60;
                currentTime += step.travelTimeMinutes + step.intervalMinutes;
            }
        }
        
        result.steps.push_back(step);
    }
    
    return result;
}

void MetroManager::displayLinesMenu() {
    if (lines.empty()) {
        std::cout << "Линий нет\n";
        return;
    }
    
    std::cout << "= Список линий =\n";
    for (size_t i = 0; i < lines.size(); ++i) {
        std::cout << (i + 1) << ". " << lines[i]->getName() 
                  << " (" << lines[i]->getColor() << ")\n";
    }
}

void MetroManager::displayStationsMenu() {
    if (stations.empty()) {
        std::cout << "Станций нет\n";
        return;
    }
    
    std::cout << "= Список станций =\n";
    for (size_t i = 0; i < stations.size(); ++i) {
        std::cout << (i + 1) << ". " << stations[i]->getName();
        const auto& lines = stations[i]->getLines();
        if (!lines.empty()) {
            std::cout << " (";
            for (size_t j = 0; j < lines.size(); ++j) {
                if (j > 0) std::cout << ", ";
                std::cout << lines[j]->getName();
            }
            std::cout << ")";
        }
        std::cout << "\n";
    }
}
    
void MetroManager::viewLine() {
    if (lines.empty()) {
        std::cout << "Линий нет\n";
        return;
    }
    
    std::cout << "= Просмотр линии =\n";
    displayLinesMenu();
    
    int choice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер линии:\n";
            choice = getValidatedInt("", 1, lines.size());
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Line* selectedLine = lines[choice - 1];
    std::cout << "Линия: " << selectedLine->getName() << " (" << selectedLine->getColor() << ")\n";
    std::cout << "Станции:\n";
    
    const auto& lineStations = selectedLine->getStations();
    for (size_t i = 0; i < lineStations.size(); ++i) {
        std::cout << i + 1 << ". ";
        lineStations[i]->displayInfo(stations);
    }
}

void MetroManager::viewSchedule() {
    if (lines.empty()) {
        std::cout << "Линий нет\n";
        return;
    }
    
    std::cout << "= Просмотр расписания =\n";
    displayLinesMenu();
    
    int choice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер линии:\n";
            choice = getValidatedInt("", 1, lines.size());
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Line* selectedLine = lines[choice - 1];
    selectedLine->displaySchedule();
}

void MetroManager::calculateRoute() {
    if (stations.empty()) {
        std::cout << "Станций нет\n";
        return;
    }
    
    std::cout << "= Расчет маршрута =\n";
    
    std::cout << "Станции:\n";
    for (size_t i = 0; i < stations.size(); ++i) {
        std::cout << (i + 1) << ". " << stations[i]->getName();
        const auto& lineNames = stations[i]->getLines();
        if (!lineNames.empty()) {
            std::cout << " (";
            for (size_t j = 0; j < lineNames.size(); ++j) {
                if (j > 0) std::cout << ", ";
                std::cout << lineNames[j]->getName();
            }
            std::cout << ")";
        }
        std::cout << "\n";
    }
    
    std::string fromName;
    std::string toName;
    int hour = 0;
    
    while (true) {
        try {
            std::cout << "Введите название станции отправления:\n";
            fromName = getValidatedString("");
            Station* from = findStationByName(fromName);
            if (!from) {
                throw StationNotFoundException(fromName);
            }
            if (from->getIsClosed()) {
                throw MetroInputException("Станция закрыта, выберите другую станцию");
            }
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    while (true) {
        try {
            std::cout << "Введите название станции назначения:\n";
            toName = getValidatedString("");
            Station* to = findStationByName(toName);
            if (!to) {
                throw StationNotFoundException(toName);
            }
            if (fromName == toName) {
                throw MetroInputException("Станции отправления и назначения должны различаться");
            }
            if (to->getIsClosed()) {
                throw MetroInputException("Станция закрыта, выберите другую станцию");
            }
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    while (true) {
        try {
            std::cout << "Введите час отправления (0-23):\n";
            hour = getValidatedHour("");
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    Station* fromStation = findStationByName(fromName);
    Station* toStation = findStationByName(toName);
    
    CalculatedRoute route = findShortestRoute(fromStation, toStation, hour);
    
    if (route.totalTimeMinutes < 0) {
        std::cout << "Маршрут не найден\n";
        return;
    }
    
    std::cout << "Маршрут найден:\n";
    std::cout << "Общее время в пути: " << std::fixed << std::setprecision(1) 
              << route.totalTimeMinutes << " мин\n";
    std::cout << "Время прибытия: " << std::setfill('0') << std::setw(2) << (hour + (int)route.totalTimeMinutes / 60) % 24 
              << ":" << std::setfill('0') << std::setw(2) << (int)route.totalTimeMinutes % 60 << "\n";
    std::cout << "Описание маршрута:\n";
    
    for (size_t i = 0; i < route.steps.size(); ++i) {
        const auto& step = route.steps[i];
        
        if (step.isTransfer) {
            std::cout << (i + 1) << ". Пересадка: " << step.fromStation << " (" << step.fromStationLine << ") -> " 
                      << step.toStation << " (" << step.toStationLine << ")\n";
            std::cout << "Время пересадки: " << std::fixed << std::setprecision(1) 
                      << step.transferTimeMinutes << " мин\n";
            std::cout << "Время прибытия: " << std::setfill('0') << std::setw(2) << step.arrivalHour 
                      << ":" << std::setfill('0') << std::setw(2) << step.arrivalMinute << "\n";
        } else {
            std::cout << (i + 1) << ". " << step.fromStation << " (" << step.lineName << ") -> " 
                      << step.toStation << " (" << step.lineName << ")\n";
            std::cout << "Линия: " << step.lineName << " (" << step.lineColor << ")\n";
            std::cout << "Время в пути: " << std::fixed << std::setprecision(1) 
                      << step.travelTimeMinutes << " мин\n";
            std::cout << "Интервал движения: " << std::fixed << std::setprecision(1) 
                      << step.intervalMinutes << " мин\n";
            std::cout << "Время прибытия: " << std::setfill('0') << std::setw(2) << step.arrivalHour 
                      << ":" << std::setfill('0') << std::setw(2) << step.arrivalMinute << "\n";
        }
    }
}

void MetroManager::addEmergency() {
    if (lines.empty() && stations.empty()) {
        std::cout << "Нет данных для создания аварийной ситуации\n";
        return;
    }
    
    std::cout << "= Добавление аварийной ситуации =\n";
    std::cout << "1. Задержка движения\n";
    std::cout << "2. Закрытие станции\n";
    
    int typeChoice = 0;
    while (true) {
        try {
            std::cout << "Выберите тип аварии:\n";
            typeChoice = getValidatedInt("", 1, 2);
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    if (typeChoice == 1) {
        if (lines.empty()) {
            std::cout << "Линий нет\n";
            return;
        }
        
        displayLinesMenu();
        std::string lineName;
        while (true) {
            try {
                std::cout << "Выберите номер линии:\n";
                int choice = getValidatedInt("", 1, lines.size());
                lineName = lines[choice - 1]->getName();
                break;
            } catch (const MetroException& e) {
                std::cerr << e.what() << "\n";
            }
        }
        
        double delay = 0;
        while (true) {
            try {
                std::cout << "Введите задержку (минут):\n";
                delay = std::stod(getValidatedString(""));
                if (delay <= 0) {
                    throw MetroInputException("Задержка должна быть положительной");
                }
                break;
            } catch (const MetroException& e) {
                std::cerr << e.what() << "\n";
            }
        }
        
        EmergencySituation* emergency = new EmergencySituation(nextEmergencyId++, "delay", lineName, delay);
        emergencySituations.push_back(emergency);
        
        Line* targetLine = findLineByName(lineName);
        if (targetLine) {
            targetLine->addDelay(delay);
        }

        saveToFile();
        
    } else if (typeChoice == 2) {
        if (stations.empty()) {
            std::cout << "Станций нет\n";
            return;
        }
        
        displayStationsMenu();
        std::string stationName;
        while (true) {
            try {
                std::cout << "Выберите номер станции:\n";
                int choice = getValidatedInt("", 1, stations.size());
                stationName = stations[choice - 1]->getName();
                break;
            } catch (const MetroException& e) {
                std::cerr << e.what() << "\n";
            }
        }
        
        EmergencySituation* emergency = new EmergencySituation(nextEmergencyId++, "closure", stationName, 0);
        emergencySituations.push_back(emergency);
        
        Station* targetStation = findStationByName(stationName);
        if (targetStation) {
            targetStation->setIsClosed(true);
        }
        
        std::cout << "Аварийная ситуация создана\n";
        saveToFile();
    }
}

void MetroManager::removeEmergency() {
    if (emergencySituations.empty()) {
        std::cout << "Аварийных ситуаций нет\n";
        return;
    }
    
    std::cout << "= Удаление аварийной ситуации =\n";
    std::cout << "Список аварийных ситуаций:\n";
    for (size_t i = 0; i < emergencySituations.size(); ++i) {
        std::cout << (i + 1) << ". ";
        emergencySituations[i]->displayInfo();
    }
    
    int choice = 0;
    while (true) {
        try {
            std::cout << "Выберите номер аварийной ситуации:\n";
            choice = getValidatedInt("", 1, emergencySituations.size());
            break;
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    EmergencySituation* toRemove = emergencySituations[choice - 1];
    
    if (toRemove->getType() == "delay") {
        Line* targetLine = findLineByName(toRemove->getLineName());
        if (targetLine) {
            targetLine->removeDelay();
        }
    } else if (toRemove->getType() == "closure") {
        Station* targetStation = findStationByName(toRemove->getStationName());
        if (targetStation) {
            targetStation->setIsClosed(false);
        }
    }
    
    delete toRemove;
    emergencySituations.erase(emergencySituations.begin() + choice - 1);
    
    std::cout << "Аварийная ситуация удалена\n";
    saveToFile();
}

int MetroManager::run(int lastAction) {
    bool taskComplete = false;
    int actionToRepeat = lastAction;
    
    while (!taskComplete) {
        if (lastAction == 0) {
            std::cout << "== МОСКОВСКОЕ МЕТРО ==\n";
            std::cout << "1. Посмотреть линию\n";
            std::cout << "2. Просмотреть расписание движения по линии\n";
            std::cout << "3. Расчет времени поездки между станциями\n";
            std::cout << "4. Добавить аварийную ситуацию\n";
            std::cout << "5. Удалить аварийную ситуацию\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие:\n";
        }
        
        try {
            if (lastAction == 0) {
                lastAction = getValidatedInt("", 0, 5);
            }
            
            switch (lastAction) {
                case 1:
                    viewLine();
                    taskComplete = true;
                    break;
                case 2:
                    viewSchedule();
                    taskComplete = true;
                    break;
                case 3:
                    calculateRoute();
                    taskComplete = true;
                    break;
                case 4:
                    addEmergency();
                    taskComplete = true;
                    break;
                case 5:
                    removeEmergency();
                    taskComplete = true;
                    break;
                case 0:
                    return 0;
            }
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        }
    }
    
    return lastAction;
}
