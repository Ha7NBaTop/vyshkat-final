#pragma once

// A capability shared by electric transport and electric inventory items.
class IEnergyUser {
public:
    virtual ~IEnergyUser() = default;
    virtual double dailyEnergyKWh() const = 0;
};
