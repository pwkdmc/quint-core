#ifndef VENDING_MACHINE_MANAGER_H
#define VENDING_MACHINE_MANAGER_H

#include <vector>
#include <string>
#include <chrono>
#include "Product.h"
#include "SalesStatistics.h"

class VendingMachineManager {
private:
    std::vector<Product*> products;
    SalesStatistics* statistics;
    std::string dataFile;
    std::chrono::system_clock::time_point currentDate;
    bool maintenanceMode;

    void displayProductsMenu();
    bool saveToFile() const;
    bool loadFromFile();
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getValidatedString(const std::string& prompt);
    std::chrono::system_clock::time_point parseDate(const std::string& input);
    std::string formatDate(const std::chrono::system_clock::time_point& date) const;
    std::string getValidatedDate(const std::string& prompt, bool mustBeFuture);
    int getNextProductId();
    void calculateChange(int amount, int& thousands, int& fiveHundreds, int& twoHundreds, 
                         int& hundreds, int& fifty, int& tens, int& fives, int& twos, int& ones);

public:
    VendingMachineManager();
    ~VendingMachineManager();

    int run(int lastAction = 0);

private:
    void viewProductsMenu();
    void addProductsMenu();
    void buyProductMenu();
    void toggleMaintenanceModeMenu();
    void showStatisticsMenu();
};

#endif
