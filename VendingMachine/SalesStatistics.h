#ifndef VENDING_MACHINE_SALES_STATISTICS_H
#define VENDING_MACHINE_SALES_STATISTICS_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <iostream>

struct SaleRecord {
    std::string productName;
    int quantity;
    double totalPrice;
};

class SalesStatistics {
private:
    std::vector<SaleRecord> sales;
    double totalRevenue;
    int totalItemsSold;

public:
    SalesStatistics() : totalRevenue(0.0), totalItemsSold(0) {
        std::cout << "\033[1;31mСоздан SalesStatistics\033[0m\n";
    }

    ~SalesStatistics() {
        std::cout << "\033[1;31mУдален SalesStatistics\033[0m\n";
    }

    void addSale(const std::string& productId, const std::string& productName,
                 int quantity, double totalPrice) {
        SaleRecord record;
        record.productName = productName;
        record.quantity = quantity;
        record.totalPrice = totalPrice;

        sales.push_back(record);
        totalRevenue += totalPrice;
        totalItemsSold += quantity;
    }

    void addSaleRecord(const SaleRecord& record) {
        sales.push_back(record);
        totalRevenue += record.totalPrice;
        totalItemsSold += record.quantity;
    }
    
    std::vector<SaleRecord>& getSales() { return sales; }
    double getTotalRevenue() const { return totalRevenue; }
    int getTotalItemsSold() const { return totalItemsSold; }

    void displayStatistics() const {
        if (sales.empty()) {
            std::cout << "Продаж нет\n";
            return;
        }

        std::cout << "Всего продано товаров: " << totalItemsSold << "\n";
        std::cout << "Общая выручка: " << totalRevenue << " руб\n";
        
        std::cout << "Продажи по товарам:\n";
        std::map<std::string, double> productNameRevenue;
        for (const auto& sale : sales) {
            productNameRevenue[sale.productName] += sale.totalPrice;
        }
        for (const auto& pair : productNameRevenue) {
            std::cout << "Название: " << pair.first << ", Выручка: " << pair.second << " руб\n";
        }

        std::cout << "История последних продаж:\n";
        size_t displayCount = sales.size() < 10 ? sales.size() : 10;
        size_t startIndex = sales.size() - displayCount;
        for (size_t i = startIndex; i < sales.size(); ++i) {
            const auto& sale = sales[i];
            
            std::cout << sale.productName 
                      << ": " << sale.quantity << " шт, " 
                      << sale.totalPrice << " руб\n";
        }
    }

    void clearStatistics() {
        sales.clear();
        totalRevenue = 0;
        totalItemsSold = 0;
    }

    std::string toJson() const {
        std::ostringstream oss;
        oss << totalRevenue << "|" << totalItemsSold << "|" << sales.size();
        for (const auto& sale : sales) {
            oss << "\n" << sale.productName << "|" 
                << sale.quantity << "|" << sale.totalPrice;
        }
        return oss.str();
    }

    static void loadFromJson(const std::string& json, SalesStatistics* stats) {
        if (stats == nullptr) return;
        
        std::istringstream iss(json);
        std::string line;
        std::getline(iss, line);
        
        std::istringstream headerStream(line);
        double revenue;
        int itemsSold;
        char delim;
        headerStream >> revenue;
        headerStream >> delim >> itemsSold;
        headerStream >> delim;
        
        stats->totalRevenue += revenue;
        stats->totalItemsSold += itemsSold;
        
        while (std::getline(iss, line)) {
            if (line.empty()) continue;
            
            std::istringstream lineStream(line);
            std::string productName;
            int quantity;
            double totalPrice;
            
            std::getline(lineStream, productName, '|');
            lineStream >> quantity >> delim >> totalPrice;
            
            SaleRecord record;
            record.productName = productName;
            record.quantity = quantity;
            record.totalPrice = totalPrice;
            
            stats->sales.push_back(record);
        }
    }
};

#endif
