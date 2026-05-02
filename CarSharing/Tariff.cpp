#include "Tariff.h"
#include <sstream>

std::string PerMinuteTariff::toJson() const {
    std::ostringstream oss;
    oss << "PERMINUTE|" << id << "|" << baseRatePerMinute << "|" << nightMultiplier;
    return oss.str();
}

std::string DailyTariff::toJson() const {
    std::ostringstream oss;
    oss << "DAILY|" << id << "|" << dailyRate << "|" << nightMultiplier;
    return oss.str();
}

std::string PackageTariff::toJson() const {
    std::ostringstream oss;
    oss << "PACKAGE|" << id << "|" << includedMinutes << "|" 
        << packagePrice << "|" << excessRate << "|" << nightMultiplier;
    return oss.str();
}

