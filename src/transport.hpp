#pragma once
#include <string_view>
#include "inventory.hpp"
#include "energy_user.hpp"
class Transport : public IInventory {
    public:
        virtual ~Transport() = default;
        InventoryNumber inventoryNumber() const override;
        std::string_view name() const override = 0;
        int simple_num() const;

    protected:
        explicit Transport( int id, int simple_num);
    
    private:
        int simple_num_;
        InventoryNumber inventory_number_;
};

class ElectricScooter : public Transport, public IEnergyUser {
public:
    ElectricScooter(int id, int simple_num, int batteryHealthPercent, double dailyEnergy);
    double dailyEnergyKWh() const override;
    std::string_view name() const override;
    int batteryHealthPercent() const;

private:
    int batteryHealthPercent_;
    double daily_energy_kwh_;
};

class Bicycle : public Transport {
public:
    Bicycle(int id, int simple_num, int brakePadWearPercent);

    std::string_view name() const override;
    int brakePadWearPercent() const;

private:
    int brakePadWearPercent_;
};
