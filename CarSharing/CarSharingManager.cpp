#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "CarSharingManager.h"
#include "exceptions.h"

std::chrono::system_clock::time_point parseDateTime(const std::string& input) {
    std::tm tm = {};
    std::istringstream ss(input);
    ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    if (ss.fail()) {
        throw InvalidDateTimeException("Неверный формат даты");
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string getValidatedDateTime(const std::string& prompt) {
    std::string value;

    std::cout << prompt << " (ДД.ММ.ГГГГ ЧЧ:ММ):\n";
    std::getline(std::cin >> std::ws, value);
    
    if (value.length() < 16) {
        throw InvalidDateTimeException("Неверный формат даты");
    }
    
    std::tm tm = {};
    std::istringstream ss(value);
    ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    
    if (ss.fail()) {
        throw InvalidDateTimeException("Неверный формат даты");
    }
    
    if (tm.tm_mday < 1 || tm.tm_mday > 31 ||
        tm.tm_mon < 0 || tm.tm_mon > 11 ||
        tm.tm_hour < 0 || tm.tm_hour > 23 ||
        tm.tm_min < 0 || tm.tm_min > 59) {
        throw InvalidDateTimeException("Неверная дата или время");
    }
    
    return value;
}

CarSharingManager::CarSharingManager() : dataFile("carsharing_data.txt") {
    std::cout << "\033[1;31mСоздан CarSharingManager\033[0m\n";
    loadFromFile();
    if (tariffs.empty()) {
        initializeDefaultTariffs();
        saveToFile();
    }
}

CarSharingManager::~CarSharingManager() {
    for (size_t i = 0; i < cars.size(); ++i) {
        delete cars[i];
    }
    cars.clear();
    
    for (size_t i = 0; i < tariffs.size(); ++i) {
        delete tariffs[i];
    }
    tariffs.clear();
    
    for (size_t i = 0; i < customers.size(); ++i) {
        delete customers[i];
    }
    customers.clear();
    
    std::cout << "\033[1;31mУдален CarSharingManager\033[0m\n";
}

void CarSharingManager::initializeDefaultTariffs() {
    tariffs.push_back(new PerMinuteTariff("T1", 15.0, 1.2));
    tariffs.push_back(new DailyTariff("T2", 2500.0, 1.2));
    tariffs.push_back(new PackageTariff("T3", 60, 800.0, 12.0, 1.2));
}

bool CarSharingManager::saveToFile() const {
    std::ofstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "CARS\n";
    for (size_t i = 0; i < cars.size(); ++i) {
        file << cars[i]->toJson() << "\n";
    }
    
    file << "TARIFFS\n";
    for (size_t i = 0; i < tariffs.size(); ++i) {
        file << tariffs[i]->toJson() << "\n";
    }
    
    file << "CUSTOMERS\n";
    for (size_t i = 0; i < customers.size(); ++i) {
        file << customers[i]->getId() << "|" << customers[i]->getName() << "|"
             << customers[i]->getAge() << "|" << customers[i]->getLicenseYears() << "|"
             << customers[i]->getBonusPoints() << "\n";
    }
    
    file.close();
    return true;
}

bool CarSharingManager::loadFromFile() {
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string section = "";
    
    while (std::getline(file, line)) {
        if (line == "CARS" || line == "TARIFFS" || line == "CUSTOMERS") {
            section = line;
            continue;
        }
        
        if (section == "CARS") {
            Car* car = Car::fromJson(line);
            if (car) {
                cars.push_back(car);
            }
        } else if (section == "CUSTOMERS") {
            std::istringstream iss(line);
            std::string id, name;
            int age, licenseYears;
            double bonusPoints;
            char delim;
            
            std::getline(iss, id, '|');
            std::getline(iss, name, '|');
            iss >> age >> delim >> licenseYears >> delim >> bonusPoints;
            
            Customer* customer = new Customer(id, name, age, licenseYears);
            customers.push_back(customer);
        }
    }
    
    file.close();
    return true;
}

int CarSharingManager::getNextCustomerId() {
    int maxId = 0;
    for (size_t i = 0; i < customers.size(); ++i) {
        std::string id = customers[i]->getId();
        if (id.length() > 1 && id[0] == 'C') {
            try {
                int num = std::stoi(id.substr(1));
                if (num > maxId) maxId = num;
            } catch (...) {}
        }
    }
    return maxId + 1;
}

int CarSharingManager::getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    std::cout << prompt;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
    if (value < min || value > max) {
        throw InvalidInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    
    return value;
}

std::string CarSharingManager::getValidatedString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty()) {
        throw InvalidInputException("Поле не может быть пустым");
    }
    return value;
}

void CarSharingManager::displayCarsMenu() {
    if (cars.empty()) {
        std::cout << "Автомобилей нет\n";
    } else {
        std::cout << "= Доступные автомобили =\n";
        for (size_t i = 0; i < cars.size(); ++i) {
            std::cout << "---\n";
            std::cout << i + 1 << ". ";
            cars[i]->displayInfo();
        }
        std::cout << "---\n";
    }
}
    
void CarSharingManager::displayTariffsMenu() {
    std::cout << "= Доступные тарифы =\n";
    for (size_t i = 0; i < tariffs.size(); ++i) {
        std::cout << "---\n";
        std::cout << i + 1 << ". ";
        tariffs[i]->displayInfo();
    }
    std::cout << "---\n";
}
    
void CarSharingManager::displayCustomersMenu() {
    if (customers.empty()) {
        std::cout << "Клиентов нет\n";
    } else {
        std::cout << "= Список клиентов =\n";
        for (size_t i = 0; i < customers.size(); ++i) {
            std::cout << "---\n";
            std::cout << i + 1 << ". ";
            std::cout << "ID: " << customers[i]->getId() << ", ";
            std::cout << "Имя: " << customers[i]->getName() << ", ";
            std::cout << "Бонусы: " << customers[i]->getBonusPoints() << "\n";
        }
        std::cout << "---\n";
    }
}

void CarSharingManager::addCustomerMenu() {
    bool complete = false;
    int step = 0;
    std::string customerName;
    int age = 0;
    int licenseYears = 0;
    std::cout << "= Добавить клиента =\n";
    while (!complete) {
        try {
            if (step == 0) {
                customerName = getValidatedString("Введите имя клиента:\n");
                step = 1;
            }
            if (step == 1) {
                std::cout << "Введите возраст:\n";
                age = getValidatedInt("", 16, 100);
                step = 2;
            }
            if (step == 2) {
                std::cout << "Введите стаж вождения (лет):\n";
                licenseYears = getValidatedInt("", 0, 40);
                step = 3;
            }
            
            Customer* customer = new Customer("C" + std::to_string(getNextCustomerId()),
                                               customerName, age, licenseYears);
            customers.push_back(customer);
            saveToFile();
            complete = true;
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void CarSharingManager::removeCustomerMenu() {
    bool complete = false;
    if (customers.empty()) {
        std::cout << "Клиентов нет\n";
        complete = true;
        return;
    }
    displayCustomersMenu();
    while (!complete) {
        try {
            std::cout << "Выберите номер клиента для удаления:\n";
            int custNum = getValidatedInt("", 1, customers.size());
            delete customers[custNum - 1];
            customers.erase(customers.begin() + custNum - 1);
            saveToFile();
            complete = true;
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}
    
Customer* CarSharingManager::getOrCreateCustomer() {
    bool complete = false;
    Customer* result = nullptr;
    int step = 0;
    int age;
    int licenseYears;
    std::string customerName;
    std::cout << "1. Выбрать существующего клиента\n";
    std::cout << "2. Создать нового клиента\n";
    while (!complete) {
        try {
            if (step == 0) {
                std::cout << "Выберите действие:\n";
                step = getValidatedInt("", 1, 2);
                if (step == 2) step = 3;
            }
            if (step == 1) {
                if (customers.empty()) {
                    std::cout << "Нет существующих клиентов\n";
                    step = 3;
                } else {
                    displayCustomersMenu();
                    step = 2;
                }
            }
            if (step == 2) {
                std::cout << "Выберите номер клиента:\n";
                int custNum = getValidatedInt("", 1, customers.size());
                result = customers[custNum - 1];
                complete = true;
            }
            if (step == 3) {
                customerName = getValidatedString("Введите имя клиента:\n");
                step = 4;
            }
            if (step == 4) {
                std::cout << "Введите возраст:\n";
                age = getValidatedInt("", 16, 100);
                step = 5;
            }
            if (step == 5) {
                std::cout << "Введите стаж вождения (лет):\n";
                licenseYears = getValidatedInt("", 0, 40);
                step = 6;
            }
            if (step == 6) {
                result = new Customer("C" + std::to_string(getNextCustomerId()),
                                       customerName, age, licenseYears);
                customers.push_back(result);
                complete = true;
            }
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    return result;
}

void CarSharingManager::addCarMenu() {
    bool complete = false;
    int step = 0;
    int typeChoice = 0;
    std::string id;
    std::string model;
    std::cout << "= Добавить автомобиль =\n";
    std::cout << "1. Эконом\n";
    std::cout << "2. Бизнес\n";
    std::cout << "3. Минивэн\n";
    std::cout << "4. Электромобиль\n";
    while (!complete) {
        try {
            if (step == 0) {
                std::cout << "Выберите тип:\n";
                typeChoice = getValidatedInt("", 1, 4);
                step = 1;
            }
            if (step == 1) {
                id = getValidatedString("Введите ID автомобиля:\n");
                step = 2;
            }
            if (step == 2) {
                model = getValidatedString("Введите модель автомобиля:\n");
                step = 3;
            }
            
            Car* newCar = nullptr;
            switch (typeChoice) {
                case 1:
                    newCar = new EconomyCar(id, model);
                    break;
                case 2:
                    newCar = new BusinessCar(id, model);
                    break;
                case 3:
                    newCar = new MinivanCar(id, model);
                    break;
                case 4:
                    newCar = new ElectricCar(id, model);
                    break;
            }
            
            if (newCar) {
                cars.push_back(newCar);
                saveToFile();
                complete = true;
            }
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void CarSharingManager::removeCarMenu() {
    bool complete = false;
    if (cars.empty()) {
        std::cout << "Автомобилей нет\n";
        complete = true;
        return;
    }
    displayCarsMenu();
    while (!complete) {
        try {
            std::cout << "Введите номер автомобиля для удаления:\n";
            int carNum = getValidatedInt("", 1, cars.size());
            
            delete cars[carNum - 1];
            cars.erase(cars.begin() + carNum - 1);
            saveToFile();
            complete = true;
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}
    
void CarSharingManager::rentCarMenu() {
    bool complete = false;
    int step = -1;
    int carChoice = 0;
    Customer* customer = nullptr;
    int tariffChoice = 0;
    std::string startDateStr;
    int rentalMinutes = 0;
    
    Car* selectedCar = nullptr;
    Tariff* selectedTariff = nullptr;
    if (cars.empty()) {
        std::cout << "Автомобилей нет\n";
        complete = true;
        return;
    }
    while (!complete) {
        try {
            if (step == -1) {
                std::cout << "= Оформление аренды =\n";
                displayCarsMenu();
                step = 0;
            }
            if (step == 0) {
                std::cout << "Выберите номер автомобиля:\n";
                carChoice = getValidatedInt("", 1, cars.size());
                selectedCar = cars[carChoice - 1];
                step = 1;
                selectedCar->displayInfo();
            }
            if (step == 1) {
                customer = getOrCreateCustomer();
                step = 2;
            }
            if (step == 2) {
                int age = customer->getAge();
                if (age < selectedCar->getMinAge()) {
                    throw InvalidAgeException();
                }
                int licenseYears = customer->getLicenseYears();
                if (licenseYears < selectedCar->getMinLicenseYears()) {
                    throw InvalidLicenseException();
                }
                step = 3;
            }
            if (step == 3) {
                displayTariffsMenu();
                step = 4;
            }
            if (step == 4) {
                std::cout << "Выберите тариф:\n";
                tariffChoice = getValidatedInt("", 1, tariffs.size());
                selectedTariff = tariffs[tariffChoice - 1];
                step = 5;
            }
            if (step == 5) {
                startDateStr = getValidatedDateTime("Введите дату начала аренды");
                step = 6;
            }
            if (step == 6) {
                if (selectedTariff->getType() == "Посуточный") {
                    std::cout << "Введите количество дней:\n";
                    int days = getValidatedInt("", 1, 365);
                    rentalMinutes = days * 24 * 60;
                } else {
                    std::cout << "Введите время аренды (минут):\n";
                    rentalMinutes = getValidatedInt("", 5, 1440);
                }
                step = 7;
            }
            
            auto startTime = parseDateTime(startDateStr);
            auto endTime = startTime + std::chrono::minutes(rentalMinutes);
            
            if (rentalMinutes <= 0) {
                throw InvalidRentalTimeException();
            }
            
            if (selectedCar->isBusyAtTime(startTime, endTime)) {
                throw CarUnavailableException(selectedCar->getId());
            }
            
            Rental* rental = new Rental(selectedCar, customer, selectedTariff, startTime, endTime);
            rental->processRental();
            rental->displayReceipt();
            
            customer->displayInfo();
            
            delete rental;
            saveToFile();
            complete = true;
        } catch (const InvalidAgeException& e) {
            std::cerr << e.what() << "\n";
            step = 1;
        } catch (const InvalidLicenseException& e) {
            std::cerr << e.what() << "\n";
            step = 1;
        } catch (const CarUnavailableException& e) {
            std::cerr << e.what() << "\n";
            step = -1;
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void CarSharingManager::carHistoryMenu() {
    bool complete = false;
    if (cars.empty()) {
        std::cout << "Автомобилей нет\n";
        complete = true;
        return;
    }

    std::cout << "= История аренды автомобиля =\n";
    displayCarsMenu();
    while (!complete) {
        try {
            std::cout << "Выберите номер автомобиля:\n";
            int carChoice = getValidatedInt("", 1, cars.size());
            Car* selectedCar = cars[carChoice - 1];
            
            const auto& history = selectedCar->getHistory();
            if (history.empty()) {
                std::cout << "История аренды пуста\n";
            } else {
                std::cout << "История для " << selectedCar->getModel() << ":\n";
                for (size_t i = 0; i < history.size(); ++i) {
                    auto start = std::chrono::system_clock::to_time_t(history[i].startTime);
                    auto end = std::chrono::system_clock::to_time_t(history[i].endTime);
                    std::tm startTm = *std::localtime(&start);
                    std::tm endTm = *std::localtime(&end);
                    
                    std::ostringstream startStr, endStr;
                    startStr << std::put_time(&startTm, "%d.%m.%Y %H:%M");
                    endStr << std::put_time(&endTm, "%d.%m.%Y %H:%M");
                    
                    std::cout << i + 1 << ". Клиент: " << history[i].customerId
                              << ", Период: " << startStr.str() << " - " << endStr.str()
                              << ", Стоимость: " << history[i].cost << " руб\n";
                }
            }
            complete = true;
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}
    
int CarSharingManager::run(int lastAction) {
    bool taskComplete = false;
    int actionToRepeat = lastAction;
    
    while (!taskComplete) {
        if (lastAction == 0) {
            std::cout << "== СИСТЕМА КАРШЕРИНГА ==\n";
            std::cout << "1. Добавить автомобиль\n";
            std::cout << "2. Удалить автомобиль\n";
            std::cout << "3. Показать все автомобили\n";
            std::cout << "4. Показать все тарифы\n";
            std::cout << "5. Добавить клиента\n";
            std::cout << "6. Удалить клиента\n";
            std::cout << "7. Показать клиентов\n";
            std::cout << "8. Оформить аренду\n";
            std::cout << "9. Показать историю аренды автомобиля\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие:\n";
        }
        
        try {
            if (lastAction == 0) {
                lastAction = getValidatedInt("", 0, 9);
            }
            
            switch (lastAction) {
                case 1:
                    addCarMenu();
                    taskComplete = true;
                    break;
                case 2:
                    removeCarMenu();
                    taskComplete = true;
                    break;
                case 3:
                    displayCarsMenu();
                    taskComplete = true;
                    break;
                case 4:
                    displayTariffsMenu();
                    taskComplete = true;
                    break;
                case 5:
                    addCustomerMenu();
                    taskComplete = true;
                    break;
                case 6:
                    removeCustomerMenu();
                    taskComplete = true;
                    break;
                case 7:
                    displayCustomersMenu();
                    taskComplete = true;
                    break;
                case 8:
                    rentCarMenu();
                    taskComplete = true;
                    break;
                case 9:
                    carHistoryMenu();
                    taskComplete = true;
                    break;
                case 0:
                    return 0;
            }
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
            lastAction = actionToRepeat;
        }
    }
        
    return lastAction;
}
