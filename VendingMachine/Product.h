#ifndef VENDING_MACHINE_PRODUCT_H
#define VENDING_MACHINE_PRODUCT_H

#include <string>
#include <chrono>
#include <sstream>
#include "exceptions.h"

enum class ProductType {
    ColdDrink,
    HotDrink,
    Snack
};

class Product {
protected:
    std::string id;
    std::string name;
    ProductType type;
    double price;
    int quantity;
    std::chrono::system_clock::time_point expirationDate;
    bool isExpired;

    std::string productTypeToString(ProductType t) const {
        switch (t) {
            case ProductType::ColdDrink: return "Холодный напиток";
            case ProductType::HotDrink: return "Горячий напиток";
            case ProductType::Snack: return "Снэк";
            default: return "Неизвестный";
        }
    }

    ProductType stringToProductType(const std::string& s) const {
        if (s == "Холодный напиток") return ProductType::ColdDrink;
        if (s == "Горячий напиток") return ProductType::HotDrink;
        if (s == "Снэк") return ProductType::Snack;
        throw VendingMachineInvalidProductTypeException();
    }

public:
    Product(const std::string& id, const std::string& name, ProductType type,
            double price, int quantity, const std::chrono::system_clock::time_point& expirationDate)
        : id(id), name(name), type(type), price(price), quantity(quantity), 
          expirationDate(expirationDate), isExpired(false) {
        std::cout << "\033[1;31mСоздан Product\033[0m\n";
    }

    virtual ~Product() {
        std::cout << "\033[1;31mУдален Product\033[0m\n";
    }

    void checkExpiration(const std::chrono::system_clock::time_point& currentDate) {
        if (currentDate >= expirationDate) {
            isExpired = true;
            quantity = 0;
        }
    }

    bool getIsExpired() const {
        return isExpired;
    }

    virtual void displayInfo() const {
        std::cout << "Название: " << name << "\n";
        std::cout << "Тип: " << productTypeToString(type) << "\n";
        std::cout << "Цена: " << price << " руб\n";
        std::cout << "Количество: " << quantity << "\n";
        if (type != ProductType::Snack) {
            std::time_t expTime = std::chrono::system_clock::to_time_t(expirationDate);
            std::tm* expTm = std::localtime(&expTime);
            char buffer[11];
            std::strftime(buffer, sizeof(buffer), "%d.%m.%Y", expTm);
            std::cout << "Срок годности: " << buffer << "\n";
        }
        if (isExpired) {
            std::cout << "СРОК ГОДНОСТИ ИСТЕК\n";
        }
    }

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    ProductType getType() const { return type; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    std::chrono::system_clock::time_point getExpirationDate() const { return expirationDate; }

    void setQuantity(int qty) {
        quantity = qty;
    }

    void addQuantity(int qty) {
        quantity += qty;
    }

    bool purchase(int amount) {
        if (isExpired) {
            return false;
        }
        if (quantity >= amount) {
            quantity -= amount;
            return true;
        }
        return false;
    }

    virtual std::string toJson() const {
        std::string typeStr;
        switch (type) {
            case ProductType::ColdDrink: typeStr = "ColdDrink"; break;
            case ProductType::HotDrink: typeStr = "HotDrink"; break;
            case ProductType::Snack: typeStr = "Snack"; break;
        }
        
        std::time_t expTime = std::chrono::system_clock::to_time_t(expirationDate);
        
        return id + "|" + name + "|" + typeStr + "|" + 
               std::to_string(price) + "|" + std::to_string(quantity) + "|" + 
               std::to_string(expTime);
    }

    static Product* fromJson(const std::string& json) {
        std::istringstream iss(json);
        std::string id, name, typeStr;
        double price;
        int quantity;
        std::time_t expTime;
        char delim;

        std::getline(iss, id, '|');
        std::getline(iss, name, '|');
        std::getline(iss, typeStr, '|');
        iss >> price >> delim >> quantity >> delim >> expTime;

        ProductType type;
        if (typeStr == "ColdDrink") type = ProductType::ColdDrink;
        else if (typeStr == "HotDrink") type = ProductType::HotDrink;
        else if (typeStr == "Snack") type = ProductType::Snack;
        else return nullptr;

        // HotDrink создается отдельно через HotDrink::fromJson
        if (type == ProductType::HotDrink) {
            return nullptr;
        }
        
        Product* product = new Product(id, name, type, price, quantity, 
                                       std::chrono::system_clock::from_time_t(expTime));
        return product;
    }
};

class HotDrink : public Product {
private:
    int temperature;

public:
    HotDrink(const std::string& id, const std::string& name, double price, 
             int quantity, const std::chrono::system_clock::time_point& expirationDate,
             int temperature = 70)
        : Product(id, name, ProductType::HotDrink, price, quantity, expirationDate), 
          temperature(temperature) {
        std::cout << "\033[1;31mСоздан HotDrink\033[0m\n";
    }

    ~HotDrink() override {
        std::cout << "\033[1;31mУдален HotDrink\033[0m\n";
    }

    int getTemperature() const { return temperature; }

    void setTemperature(int temp) {
        if (temp < 60 || temp > 95) {
            throw VendingMachineInvalidInputException("Температура должна быть от 60 до 95 градусов");
        }
        temperature = temp;
    }

    void displayInfo() const override {
        Product::displayInfo();
        std::cout << "Температура: " << temperature << "°C\n";
    }

    std::string toJson() const override {
        std::string base = Product::toJson();
        return base + "|" + std::to_string(temperature);
    }

    static HotDrink* fromJson(const std::string& json) {
        std::istringstream iss(json);
        std::string id, name, typeStr;
        double price;
        int quantity, temperature;
        std::time_t expTime;
        char delim;

        std::getline(iss, id, '|');
        std::getline(iss, name, '|');
        std::getline(iss, typeStr, '|');
        iss >> price >> delim >> quantity >> delim >> expTime >> delim >> temperature;

        HotDrink* drink = new HotDrink(id, name, price, quantity, 
                                       std::chrono::system_clock::from_time_t(expTime),
                                       temperature);
        return drink;
    }
};

#endif
