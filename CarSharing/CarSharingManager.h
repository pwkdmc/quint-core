#ifndef CARSHARING_MANAGER_H
#define CARSHARING_MANAGER_H

#include <vector>
#include "Car.h"
#include "Tariff.h"
#include "Customer.h"
#include "Rental.h"
#include <string>

class CarSharingManager {
private:
    std::vector<Car*> cars;
    std::vector<Tariff*> tariffs;
    std::vector<Customer*> customers;
    std::string dataFile;
    
    void initializeDefaultTariffs();
    void displayCarsMenu();
    void displayTariffsMenu();
    void displayCustomersMenu();
    bool saveToFile() const;
    bool loadFromFile();
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getValidatedString(const std::string& prompt);
    Customer* getOrCreateCustomer();
    int getNextCustomerId();
    
public:
    CarSharingManager();
    ~CarSharingManager();
    
    int run(int lastAction = 0);
    
private:
    void addCarMenu();
    void removeCarMenu();
    void addCustomerMenu();
    void removeCustomerMenu();
    void rentCarMenu();
    void carHistoryMenu();
};

#endif
