#pragma once
#include "inventory.hpp"
#include "energy_user.hpp"
#include <string_view>

class Item : public IInventory {
public:

    InventoryNumber inventoryNumber() const override;
    std::string_view name() const override = 0;

protected:
    explicit Item(int id);

private:
    InventoryNumber inventory_number_;
};

class Helmet : public Item {
public:
    explicit Helmet(int id);
    std::string_view name() const override;
};

class ChargingStation : public Item, public IEnergyUser {
public:
    ChargingStation(int id, double dailyEnergy);
    std::string_view name() const override;
    double dailyEnergyKWh() const override;

private:
    double daily_energy_kwh_;
};
