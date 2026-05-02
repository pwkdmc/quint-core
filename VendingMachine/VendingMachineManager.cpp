#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "VendingMachineManager.h"
#include "Product.h"
#include "exceptions.h"

std::chrono::system_clock::time_point parseDate(const std::string& input) {
    std::tm tm = {};
    std::istringstream ss(input);
    ss >> std::get_time(&tm, "%d.%m.%Y");
    if (ss.fail()) {
        throw VendingMachineInvalidDateException("Неверный формат даты");
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
    
std::string formatDate(const std::chrono::system_clock::time_point& date) {
    std::time_t t = std::chrono::system_clock::to_time_t(date);
    std::tm* tm = std::localtime(&t);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y", tm);
    return std::string(buffer);
}

VendingMachineManager::VendingMachineManager() 
    : dataFile("vendingmachine_data.txt"), maintenanceMode(false) {
    std::cout << "\033[1;31mСоздан VendingMachineManager\033[0m\n";
    statistics = new SalesStatistics();
    bool loaded = loadFromFile();
    
    std::string dateStr;
    while (true) {
        std::cout << "Введите текущую дату (ДД.ММ.ГГГГ):\n";
        std::getline(std::cin >> std::ws, dateStr);
        
        if (dateStr.length() != 10) {
            std::cerr << "Ошибка: дата должна быть в формате ДД.ММ.ГГГГ (10 символов)\n";
            continue;
        }
        
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%d.%m.%Y");
        
        if (ss.fail() || !ss.eof()) {
            std::cerr << "Ошибка: неверный формат даты. Используйте ДД.ММ.ГГГГ\n";
            continue;
        }
        
        int day = tm.tm_mday;
        int month = tm.tm_mon + 1;
        int year = tm.tm_year + 1900;
        
        if (day < 1 || month < 1 || month > 12) {
            std::cerr << "Ошибка: неверная дата\n";
            continue;
        }
        
        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) daysInMonth[1] = 29;
        
        if (day > daysInMonth[month - 1]) {
            std::cerr << "Ошибка: неверная дата\n";
            continue;
        }
        
        currentDate = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        break;
    }
    
    for (size_t i = 0; i < products.size(); ) {
        products[i]->checkExpiration(currentDate);
        if (products[i]->getIsExpired()) {
            delete products[i];
            products.erase(products.begin() + i);
        } else {
            ++i;
        }
    }
    
    saveToFile();
}

VendingMachineManager::~VendingMachineManager() {
    saveToFile();
    
    for (size_t i = 0; i < products.size(); ++i) {
        delete products[i];
    }
    products.clear();
    
    delete statistics;
    std::cout << "\033[1;31mУдален VendingMachineManager\033[0m\n";
}

int VendingMachineManager::getNextProductId() {
    int maxId = 0;
    for (size_t i = 0; i < products.size(); ++i) {
        std::string id = products[i]->getId();
        if (id.length() > 1 && id[0] == 'P') {
            try {
                int num = std::stoi(id.substr(1));
                if (num > maxId) maxId = num;
            } catch (...) {}
        }
    }
    return maxId + 1;
}
    
int VendingMachineManager::getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    std::cout << prompt;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw VendingMachineInvalidInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (value < min || value > max) {
        throw VendingMachineInvalidInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    
    return value;
}

std::string VendingMachineManager::getValidatedString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty()) {
        throw VendingMachineInvalidInputException("Поле не может быть пустым");
    }
    return value;
}
    
std::chrono::system_clock::time_point VendingMachineManager::parseDate(const std::string& input) {
    return ::parseDate(input);
}

std::string VendingMachineManager::formatDate(const std::chrono::system_clock::time_point& date) const {
    return ::formatDate(date);
}

std::string VendingMachineManager::getValidatedDate(const std::string& prompt, bool mustBeFuture) {
    std::string value;
    std::cout << prompt << " (ДД.ММ.ГГГГ):\n";
    std::getline(std::cin >> std::ws, value);
    
    if (value.length() != 10) {
        throw VendingMachineInvalidDateException("Дата должна быть в формате ДД.ММ.ГГГГ (10 символов)");
    }
    
    std::tm tm = {};
    std::istringstream ss(value);
    ss >> std::get_time(&tm, "%d.%m.%Y");
    
    if (ss.fail() || !ss.eof()) {
        throw VendingMachineInvalidDateException("Неверный формат даты");
    }
    
    int day = tm.tm_mday;
    int month = tm.tm_mon + 1;
    int year = tm.tm_year + 1900;
    
    if (day < 1 || month < 1 || month > 12) {
        throw VendingMachineInvalidDateException("Неверная дата");
    }
    
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeap) daysInMonth[1] = 29;
    
    if (day > daysInMonth[month - 1]) {
        throw VendingMachineInvalidDateException("Неверная дата");
    }
    
    std::chrono::system_clock::time_point inputDate = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    if (mustBeFuture && inputDate <= currentDate) {
        throw VendingMachineInvalidDateException("Срок годности должен быть больше текущей даты");
    }
    
    return value;
}

void VendingMachineManager::displayProductsMenu() {
    if (products.empty()) {
        std::cout << "Товаров нет\n";
        return;
    }
    
    std::cout << "= Список товаров =\n";
    for (size_t i = 0; i < products.size(); ++i) {
        std::cout << "---\n";
        std::cout << i + 1 << ". ";
        products[i]->displayInfo();
    }
    std::cout << "---\n";
}

bool VendingMachineManager::saveToFile() const {
    std::ofstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::time_t date = std::chrono::system_clock::to_time_t(currentDate);
    file << "CURRENT_DATE:" << date << "\n";
    file << "MAINTENANCE:" << (maintenanceMode ? "1" : "0") << "\n";
    
    file << "PRODUCT_COUNT:" << products.size() << "\n";
    for (size_t i = 0; i < products.size(); ++i) {
        file << products[i]->toJson() << "\n";
    }
    
    file << "STATISTICS:" << statistics->toJson() << "\n";
    
    file.close();
    return true;
}

bool VendingMachineManager::loadFromFile() {
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool readingStatistics = false;
    bool loadedStatistics = false;
    
    while (std::getline(file, line)) {
        if (line.find("CURRENT_DATE:") == 0) {
            std::time_t date;
            std::istringstream iss(line.substr(13));
            iss >> date;
            currentDate = std::chrono::system_clock::from_time_t(date);
        } else if (line.find("MAINTENANCE:") == 0) {
            maintenanceMode = (line.substr(12) == "1");
        } else if (line.find("PRODUCT_COUNT:") == 0) {
            int count = std::stoi(line.substr(14));
            for (int i = 0; i < count; ++i) {
                if (std::getline(file, line)) {
                    std::istringstream iss(line);
                    std::string id, name, typeStr;
                    std::getline(iss, id, '|');
                    std::getline(iss, name, '|');
                    std::getline(iss, typeStr, '|');
                    
                    Product* product = nullptr;
                    if (typeStr == "HotDrink") {
                        product = HotDrink::fromJson(line);
                    } else {
                        product = Product::fromJson(line);
                    }
                    
                    if (product) {
                        products.push_back(product);
                    }
                }
            }
        } else if (line.find("STATISTICS:") == 0) {
            readingStatistics = true;
            std::string statsData = line.substr(11);
            if (!statsData.empty()) {
                SalesStatistics::loadFromJson(statsData, statistics);
                loadedStatistics = true;
            }
        } else if (readingStatistics && !line.empty()) {
            std::string statsData = "0|0|0\n" + line;
            SalesStatistics::loadFromJson(statsData, statistics);
        }
    }
    
    file.close();
    return loadedStatistics;
}

void VendingMachineManager::calculateChange(int amount, int& thousands, int& fiveHundreds, 
                                            int& twoHundreds, int& hundreds, int& fifty, 
                                            int& tens, int& fives, int& twos, int& ones) {
    int remaining = amount;
    
    thousands = remaining / 1000;
    remaining %= 1000;
    
    fiveHundreds = remaining / 500;
    remaining %= 500;
    
    twoHundreds = remaining / 200;
    remaining %= 200;
    
    hundreds = remaining / 100;
    remaining %= 100;
    
    fifty = remaining / 50;
    remaining %= 50;
    
    tens = remaining / 10;
    remaining %= 10;
    
    fives = remaining / 5;
    remaining %= 5;
    
    twos = remaining / 2;
    remaining %= 2;
    
    ones = remaining;
}

void VendingMachineManager::viewProductsMenu() {
    bool complete = false;
    while (!complete) {
        try {
            std::cout << "= Просмотр товаров =\n";
            displayProductsMenu();
            complete = true;
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void VendingMachineManager::addProductsMenu() {
    bool complete = false;
    int step = 0;
    int productChoice = 0;
    std::string productName;
    int productTypeChoice = 0;
    std::string expirationDateStr;
    int quantity = 0;
    double price = 0;
    int temperature = 70;

    while (!complete) {
        try {
            if (step == 0) {
                std::cout << "= Добавление товаров =\n";
                std::cout << "1. Пополнить существующий товар\n";
                std::cout << "2. Создать новый товар\n";
                std::cout << "Выберите действие:\n";
                productChoice = getValidatedInt("", 1, 2);
                step = 1;
            }
            if (step == 1 && productChoice == 1) {
                if (products.empty()) {
                    std::cout << "Нет существующих товаров\n";
                    step = 0;
                    continue;
                }
                displayProductsMenu();
                step = 2;
            }
            if (step == 2 && productChoice == 1) {
                std::cout << "Выберите номер товара:\n";
                int productNum = getValidatedInt("", 1, products.size());
                int addQty = 0;
                std::cout << "Введите количество для добавления:\n";
                addQty = getValidatedInt("", 1, 1000);
                products[productNum - 1]->addQuantity(addQty);
                std::cout << "Товар пополнен\n";
                complete = true;
            }
            if (step == 1 && productChoice == 2) {
                std::cout << "1. Холодный напиток\n";
                std::cout << "2. Горячий напиток\n";
                std::cout << "3. Снэк\n";
                std::cout << "Выберите тип товара:\n";
                productTypeChoice = getValidatedInt("", 1, 3);
                step = 3;
            }
            if (step == 3 && productChoice == 2) {
                productName = getValidatedString("Введите название товара:\n");
                step = 4;
            }
            if (step == 4 && productChoice == 2) {
                expirationDateStr = getValidatedDate("Введите срок годности", true);
                step = 5;
            }
            if (step == 5 && productChoice == 2) {
                std::cout << "Введите количество товара:\n";
                quantity = getValidatedInt("", 1, 1000);
                step = 6;
            }
            if (step == 6 && productChoice == 2) {
                std::cout << "Введите цену за 1 шт:\n";
                price = getValidatedInt("", 1, 10000);
                step = 7;
            }
            if (step == 7 && productChoice == 2 && productTypeChoice == 2) {
                std::cout << "Введите температуру напитка (60-95):\n";
                temperature = getValidatedInt("", 60, 95);
                step = 8;
            }
            if (step == 7 && productChoice == 2 && productTypeChoice != 2) {
                ProductType type;
                if (productTypeChoice == 1) type = ProductType::ColdDrink;
                else type = ProductType::Snack;
                
                std::chrono::system_clock::time_point expDate = parseDate(expirationDateStr);
                
                int existingIndex = -1;
                for (size_t i = 0; i < products.size(); ++i) {
                    if (products[i]->getName() == productName && 
                        products[i]->getType() == type && 
                        products[i]->getPrice() == price && 
                        products[i]->getExpirationDate() == expDate) {
                        existingIndex = i;
                        break;
                    }
                }
                
                if (existingIndex >= 0) {
                    products[existingIndex]->addQuantity(quantity);
                    std::cout << "Товар пополнен\n";
                } else {
                    Product* newProduct = new Product("P" + std::to_string(getNextProductId()), 
                                                     productName, type, price, quantity, expDate);
                    
                    products.push_back(newProduct);
                    std::cout << "Товар создан\n";
                }
                complete = true;
            }
            if (step == 8 && productChoice == 2 && productTypeChoice == 2) {
                ProductType type = ProductType::HotDrink;
                
                std::chrono::system_clock::time_point expDate = parseDate(expirationDateStr);
                
                int existingIndex = -1;
                for (size_t i = 0; i < products.size(); ++i) {
                    HotDrink* hotDrink = dynamic_cast<HotDrink*>(products[i]);
                    if (hotDrink && 
                        hotDrink->getName() == productName && 
                        hotDrink->getPrice() == price && 
                        hotDrink->getExpirationDate() == expDate &&
                        hotDrink->getTemperature() == temperature) {
                        existingIndex = i;
                        break;
                    }
                }
                
                if (existingIndex >= 0) {
                    products[existingIndex]->addQuantity(quantity);
                    std::cout << "Товар пополнен\n";
                } else {
                    Product* newProduct = new HotDrink("P" + std::to_string(getNextProductId()), 
                                                      productName, price, quantity, expDate, temperature);
                    
                    products.push_back(newProduct);
                    std::cout << "Товар создан\n";
                }
                complete = true;
            }
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void VendingMachineManager::buyProductMenu() {
    if (maintenanceMode) {
        std::cerr << "Ошибка: Автомат в режиме техобслуживания\n";
        return;
    }
    
    if (products.empty()) {
        std::cout << "Товаров нет\n";
        return;
    }
    
    bool complete = false;
    int step = 0;
    int productNum = 0;
    int buyQuantity = 0;
    double payment = 0;
    
    std::cout << "= Покупка товара =\n";
    
    while (!complete) {
        try {
            if (step == 0) {
                displayProductsMenu();
                step = 1;
            }
            if (step == 1) {
                std::cout << "Выберите номер товара:\n";
                productNum = getValidatedInt("", 1, products.size());
                step = 2;
            }
            if (step == 2) {
                std::cout << "Введите количество:\n";
                buyQuantity = getValidatedInt("", 1, products[productNum - 1]->getQuantity());
                step = 3;
            }
            if (step == 3) {
                double productPrice = products[productNum - 1]->getPrice();
                double totalCost = productPrice * buyQuantity;
                std::cout << "Общая стоимость: " << totalCost << " руб\n";
                std::cout << "Внесите деньги (монеты 1, 2, 5 или купюры 10, 20, 50, 100, 200, 500, 1000):\n";
                std::cout << "1. Монета 1 руб\n";
                std::cout << "2. Монета 2 руб\n";
                std::cout << "3. Монета 5 руб\n";
                std::cout << "4. Купюра 10 руб\n";
                std::cout << "5. Купюра 20 руб\n";
                std::cout << "6. Купюра 50 руб\n";
                std::cout << "7. Купюра 100 руб\n";
                std::cout << "8. Купюра 200 руб\n";
                std::cout << "9. Купюра 500 руб\n";
                std::cout << "10. Купюра 1000 руб\n";
                std::cout << "0. Готово\n";
                step = 4;
            }
            if (step == 4) {
                int paymentChoice = getValidatedInt("", 0, 10);
                if (paymentChoice == 0) {
                    if (payment < products[productNum - 1]->getPrice() * buyQuantity) {
                        std::cout << "Недостаточно средств\n";
                        step = 4;
                        continue;
                    }
                    complete = true;
                } else {
                    int amount = 0;
                    switch (paymentChoice) {
                        case 1: amount = 1; break;
                        case 2: amount = 2; break;
                        case 3: amount = 5; break;
                        case 4: amount = 10; break;
                        case 5: amount = 20; break;
                        case 6: amount = 50; break;
                        case 7: amount = 100; break;
                        case 8: amount = 200; break;
                        case 9: amount = 500; break;
                        case 10: amount = 1000; break;
                    }
                    payment += amount;
                    std::cout << "Внесено: " << amount << " руб\n";
                    std::cout << "Всего внесено: " << payment << " руб\n";
                }
            }
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
            payment = 0;
            step = 0;
        }
    }
    
    try {
        double productPrice = products[productNum - 1]->getPrice();
        double totalCost = productPrice * buyQuantity;
        double change = payment - totalCost;
        
        products[productNum - 1]->purchase(buyQuantity);
        
        statistics->addSale(products[productNum - 1]->getId(), 
                           products[productNum - 1]->getName(), 
                           buyQuantity, totalCost);
        
        if (products[productNum - 1]->getQuantity() == 0) {
            delete products[productNum - 1];
            products.erase(products.begin() + productNum - 1);
        }
        
        std::cout << "Товар выдан\n";
        if (change > 0) {
            std::cout << "Сдача: " << change << " руб\n";
            int thousands, fiveHundreds, twoHundreds, hundreds, fifty, tens, fives, twos, ones;
            calculateChange(static_cast<int>(change), thousands, fiveHundreds, twoHundreds, 
                           hundreds, fifty, tens, fives, twos, ones);
                        
            std::cout << "Сдача монетами/купюрами:\n";
            if (thousands > 0) std::cout << "1000 руб: " << thousands << "\n";
            if (fiveHundreds > 0) std::cout << "500 руб: " << fiveHundreds << "\n";
            if (twoHundreds > 0) std::cout << "200 руб: " << twoHundreds << "\n";
            if (hundreds > 0) std::cout << "100 руб: " << hundreds << "\n";
            if (fifty > 0) std::cout << "50 руб: " << fifty << "\n";
            if (tens > 0) std::cout << "10 руб: " << tens << "\n";
            if (fives > 0) std::cout << "5 руб: " << fives << "\n";
            if (twos > 0) std::cout << "2 руб: " << twos << "\n";
            if (ones > 0) std::cout << "1 руб: " << ones << "\n";
        }
        
        saveToFile();
    } catch (const VendingMachineException& e) {
        std::cerr << e.what() << "\n";
    }
}

void VendingMachineManager::toggleMaintenanceModeMenu() {
    bool complete = false;
    while (!complete) {
        try {
            std::cout << "= Режим техобслуживания =\n";
            std::cout << "Текущее состояние: " << (maintenanceMode ? "Включен" : "Выключен") << "\n";
            std::cout << "1. Включить\n";
            std::cout << "2. Выключить\n";
            std::cout << "Выберите действие:\n";
            int choice = getValidatedInt("", 1, 2);
            
            if (choice == 1) {
                maintenanceMode = true;
                std::cout << "Режим техобслуживания включен\n";
            } else {
                maintenanceMode = false;
                std::cout << "Режим техобслуживания выключен\n";
            }
            
            saveToFile();
            complete = true;
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void VendingMachineManager::showStatisticsMenu() {
    bool complete = false;
    while (!complete) {
        try {
            std::cout << "= Статистика продаж =\n";
            statistics->displayStatistics();
            complete = true;
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

int VendingMachineManager::run(int lastAction) {
    bool taskComplete = false;
    int actionToRepeat = lastAction;
    
    while (!taskComplete) {
        if (lastAction == 0) {
            std::time_t date = std::chrono::system_clock::to_time_t(currentDate);
            std::tm* tm = std::localtime(&date);
            char buffer[11];
            std::strftime(buffer, sizeof(buffer), "%d.%m.%Y", tm);
            
            std::cout << "== ТОРГОВЫЙ АВТОМАТ ==\n";
            std::cout << "Дата: " << buffer << "\n";
            std::cout << "1. Посмотреть товары\n";
            std::cout << "2. Добавить товары\n";
            std::cout << "3. Купить товар\n";
            std::cout << "4. Включить/выключить режим техобслуживания\n";
            std::cout << "5. Вывести статистику продаж\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие:\n";
        }
        
        try {
            if (lastAction == 0) {
                lastAction = getValidatedInt("", 0, 5);
            }
            
            switch (lastAction) {
                case 1:
                    viewProductsMenu();
                    taskComplete = true;
                    break;
                case 2:
                    addProductsMenu();
                    saveToFile();
                    taskComplete = true;
                    break;
                case 3:
                    buyProductMenu();
                    saveToFile();
                    taskComplete = true;
                    break;
                case 4:
                    toggleMaintenanceModeMenu();
                    saveToFile();
                    taskComplete = true;
                    break;
                case 5:
                    showStatisticsMenu();
                    taskComplete = true;
                    break;
                case 0:
                    return 0;
            }
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
            lastAction = actionToRepeat;
        }
    }
    
    return lastAction;
}
