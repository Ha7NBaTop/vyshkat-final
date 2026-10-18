#include "transport.hpp"
#include <stdexcept>
#include <cmath>

ElectricScooter::ElectricScooter(int id, int simple_num,int batteryHealthPercent, double dailyEnergy)
    : Transport(id, simple_num),
      batteryHealthPercent_(batteryHealthPercent), daily_energy_kwh_(dailyEnergy) {
        if(batteryHealthPercent < 0 || batteryHealthPercent > 100){
            throw std::invalid_argument("Battery health percents must be between 0 and 100");
        }
        if(dailyEnergy < 0 || !std::isfinite(dailyEnergy)){
            throw std::invalid_argument("Daily energy must be Real non-negative number");
        }
}

Bicycle::Bicycle(int id, int simple_num, int brakePadWearPercent)
    : Transport(id, simple_num),
      brakePadWearPercent_(brakePadWearPercent) {
        if(brakePadWearPercent < 0 || brakePadWearPercent > 100){
            throw std::invalid_argument("Brake pad wear percents must be between 0 and 100");
        }
}

Transport::Transport(int id, int simple_num) : simple_num_(simple_num), inventory_number_(id){
    if(simple_num < 1 || simple_num > 10){
        throw std::invalid_argument("simplicity must be between 1 and 10");
    }
}

std::string_view Bicycle::name() const{
    return "Bicycle";
}
std::string_view ElectricScooter::name() const{
    return "Electric Scooter";
}
InventoryNumber Transport::inventoryNumber() const{
    return inventory_number_;
}
double ElectricScooter::dailyEnergyKWh() const{
    return daily_energy_kwh_;
}
int Transport::simple_num() const{
    return simple_num_;
}

int Bicycle::brakePadWearPercent() const{
    return brakePadWearPercent_;
}

int ElectricScooter::batteryHealthPercent() const{
    return batteryHealthPercent_;
}
