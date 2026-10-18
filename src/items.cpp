#include "items.hpp"
#include <cmath>
#include <stdexcept>

Item::Item(int id): inventory_number_(id) {}

Helmet::Helmet(int id): Item(id) {}

std::string_view Helmet::name() const {
    return "Helmet";
}

InventoryNumber Item::inventoryNumber() const{
    return inventory_number_;
}

ChargingStation::ChargingStation(int id, double dailyEnergy)
    : Item(id), daily_energy_kwh_(dailyEnergy) {
    if (!std::isfinite(dailyEnergy) || dailyEnergy < 0) {
        throw std::invalid_argument("Daily energy must be finite and non-negative");
    }
}

std::string_view ChargingStation::name() const {
    return "Charging Station";
}

double ChargingStation::dailyEnergyKWh() const {
    return daily_energy_kwh_;
}
