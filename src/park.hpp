#pragma once

#include "items.hpp"
#include "service_center.hpp"

#include <cstddef>
#include <memory>
#include <vector>

struct ParkReport {
    std::size_t transportCount = 0;
    std::size_t itemCount = 0;
    std::size_t totalCount = 0;
    double dailyEnergyKWh = 0;
};

class Park {
public:
    // The injected service center must outlive the park.
    explicit Park(IServiceCenter& serviceCenter);
    bool addTransport(std::unique_ptr<Transport> transport);
    void addItem(std::unique_ptr<Item> item);
    ParkReport report() const;
    std::vector<const Transport*> beginners() const;
    std::vector<const IInventory*> inventory() const;

private:
    void validateNewInventory(const IInventory& object) const;
    double totalEnergyWith(const Transport* transport = nullptr,
                           const Item* item = nullptr) const;

    IServiceCenter& serviceCenter_;
    std::vector<std::unique_ptr<Transport>> transports_;
    std::vector<std::unique_ptr<Item>> items_;
};
