#ifndef CARSHARING_TARIFF_H
#define CARSHARING_TARIFF_H

#include <string>
#include <iostream>
#include <chrono>

class Tariff {
protected:
    std::string id;
    std::string name;
    double baseRatePerMinute;
    double nightMultiplier;
    
    bool isNightHour(int hourOfDay) const {
        return hourOfDay >= 22 || hourOfDay < 6;
    }
    
public:
    Tariff(const std::string& id, const std::string& name, 
           double baseRatePerMinute, double nightMultiplier = 1.2)
        : id(id), name(name), baseRatePerMinute(baseRatePerMinute), 
          nightMultiplier(nightMultiplier) {
        std::cout << "\033[1;31mСоздан Tariff\033[0m\n";
    }
    
    virtual ~Tariff() {
        std::cout << "\033[1;31mУдален Tariff\033[0m\n";
    }
    
    virtual double calculateCost(const std::chrono::system_clock::time_point& startTime,
                                  const std::chrono::system_clock::time_point& endTime) const {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        int totalMinutes = static_cast<int>(duration.count());
        
        if (totalMinutes <= 0) return 0.0;
        
        double totalCost = 0.0;
        auto current = startTime;
        auto end = endTime;
        
        while (current < end) {
            auto next = current + std::chrono::minutes(1);
            auto time_t = std::chrono::system_clock::to_time_t(current);
            std::tm* tm = std::localtime(&time_t);
            int hourOfDay = tm->tm_hour;
            
            double hourMultiplier = isNightHour(hourOfDay) ? nightMultiplier : 1.0;
            totalCost += baseRatePerMinute * hourMultiplier;
            
            current = next;
        }
        
        return totalCost;
    }
    
    virtual std::string getType() const = 0;
    
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    
    virtual void displayInfo() const {
        std::cout << "Тариф: " << name << "\n";
        std::cout << "Базовая ставка: " << baseRatePerMinute << " руб/мин\n";
        std::cout << "Ночной множитель: " << nightMultiplier << "\n";
    }
    
    virtual std::string toJson() const = 0;
};

class PerMinuteTariff : public Tariff {
public:
    PerMinuteTariff(const std::string& id, double ratePerMinute, double nightMultiplier = 1.2)
        : Tariff(id, "Поминутный", ratePerMinute, nightMultiplier) {
        std::cout << "\033[1;31mСоздан PerMinuteTariff\033[0m\n";
    }
    
    ~PerMinuteTariff() override {
        std::cout << "\033[1;31mУдален PerMinuteTariff\033[0m\n";
    }
    
    std::string getType() const override { return "Поминутный"; }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime) const override {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        int totalMinutes = static_cast<int>(duration.count());
        
        if (totalMinutes <= 0) return 0.0;
        
        double totalCost = 0.0;
        auto current = startTime;
        auto end = endTime;
        
        while (current < end) {
            auto next = current + std::chrono::minutes(1);
            auto time_t = std::chrono::system_clock::to_time_t(current);
            std::tm* tm = std::localtime(&time_t);
            int hourOfDay = tm->tm_hour;
            
            double hourMultiplier = isNightHour(hourOfDay) ? nightMultiplier : 1.0;
            totalCost += baseRatePerMinute * hourMultiplier;
            
            current = next;
        }
        
        return totalCost;
    }
    
    void displayInfo() const override {
        Tariff::displayInfo();
        std::cout << "Оплачивается за каждую минуту использования\n";
    }
    
    std::string toJson() const override;
};

class DailyTariff : public Tariff {
private:
    double dailyRate;
    
public:
    DailyTariff(const std::string& id, double dailyRate, double nightMultiplier = 1.2)
        : Tariff(id, "Посуточный", dailyRate / 1440.0, nightMultiplier), 
          dailyRate(dailyRate) {
        std::cout << "\033[1;31mСоздан DailyTariff\033[0m\n";
    }
    
    ~DailyTariff() override {
        std::cout << "\033[1;31mУдален DailyTariff\033[0m\n";
    }
    
    std::string getType() const override { return "Посуточный"; }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime) const override {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        int totalMinutes = static_cast<int>(duration.count());
        
        if (totalMinutes <= 0) return 0.0;
        
        int days = (totalMinutes + 1439) / 1440;
        double baseCost = dailyRate * days;
        
        int nightMinutes = 0;
        int dayMinutes = 0;
        auto current = startTime;
        auto end = endTime;
        
        while (current < end) {
            auto next = current + std::chrono::minutes(1);
            auto time_t = std::chrono::system_clock::to_time_t(current);
            std::tm* tm = std::localtime(&time_t);
            int hourOfDay = tm->tm_hour;
            
            if (isNightHour(hourOfDay)) {
                nightMinutes++;
            } else {
                dayMinutes++;
            }
            current = next;
        }
        
        double dayRate = baseCost * static_cast<double>(dayMinutes) / totalMinutes;
        double nightRate = baseCost * static_cast<double>(nightMinutes) / totalMinutes * nightMultiplier;
        
        return dayRate + nightRate;
    }
    
    void displayInfo() const override {
        std::cout << "Тариф: " << name << "\n";
        std::cout << "Суточная ставка: " << dailyRate << " руб/сутки\n";
        std::cout << "Ночной множитель: " << nightMultiplier << "\n";
    }
    
    double getDailyRate() const { return dailyRate; }
    
    std::string toJson() const override;
};

class PackageTariff : public Tariff {
private:
    int includedMinutes;
    double packagePrice;
    double excessRate;
    
public:
    PackageTariff(const std::string& id, int includedMinutes, double packagePrice,
                  double excessRate = 5.0, double nightMultiplier = 1.2)
        : Tariff(id, "Пакетный", 0, nightMultiplier),
          includedMinutes(includedMinutes), packagePrice(packagePrice),
          excessRate(excessRate) {
        std::cout << "\033[1;31mСоздан PackageTariff\033[0m\n";
    }
    
    ~PackageTariff() override {
        std::cout << "\033[1;31mУдален PackageTariff\033[0m\n";
    }
    
    std::string getType() const override { return "Пакетный"; }
    
    double calculateCost(const std::chrono::system_clock::time_point& startTime,
                         const std::chrono::system_clock::time_point& endTime) const override {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        int totalMinutes = static_cast<int>(duration.count());
        
        if (totalMinutes <= 0) return 0.0;
        
        int nightMinutes = 0;
        int dayMinutes = 0;
        auto current = startTime;
        auto end = endTime;
        
        while (current < end) {
            auto next = current + std::chrono::minutes(1);
            auto time_t = std::chrono::system_clock::to_time_t(current);
            std::tm* tm = std::localtime(&time_t);
            int hourOfDay = tm->tm_hour;
            
            if (isNightHour(hourOfDay)) {
                nightMinutes++;
            } else {
                dayMinutes++;
            }
            current = next;
        }
        
        double dayCost, nightCost;
        if (totalMinutes <= includedMinutes) {
            dayCost = packagePrice * static_cast<double>(dayMinutes) / totalMinutes;
            nightCost = packagePrice * static_cast<double>(nightMinutes) / totalMinutes * nightMultiplier;
        } else {
            int excessMinutes = totalMinutes - includedMinutes;
            dayCost = packagePrice * static_cast<double>(dayMinutes) / totalMinutes;
            nightCost = (packagePrice * static_cast<double>(nightMinutes) / totalMinutes + 
                        excessRate * excessMinutes * static_cast<double>(nightMinutes) / totalMinutes) * nightMultiplier;
        }
        
        return dayCost + nightCost;
    }
    
    void displayInfo() const override {
        Tariff::displayInfo();
        std::cout << "Включено минут: " << includedMinutes << "\n";
        std::cout << "Стоимость пакета: " << packagePrice << " руб\n";
        std::cout << "Стоимость превышения: " << excessRate << " руб/мин\n";
    }
    
    std::string toJson() const override;
};

#endif
