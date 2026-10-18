#include "park.hpp"

#include <cmath>
#include <stdexcept>

namespace {
double energyOf(const IInventory& object) {
    const auto* consumer = dynamic_cast<const IEnergyUser*>(&object);
    return consumer ? consumer->dailyEnergyKWh() : 0.0;
}
}

Park::Park(IServiceCenter& serviceCenter) : serviceCenter_(serviceCenter) {}

double Park::totalEnergyWith(const Transport* transport, const Item* item) const {
    double total = 0;
    const auto add = [&total](const IInventory& object) {
        total += energyOf(object);
        if (!std::isfinite(total)) {
            throw std::invalid_argument("Total daily energy exceeds the numeric range");
        }
    };
    // Match report order, including the proposed object before committing ownership.
    for (const auto& existing : transports_) {
        add(*existing);
    }
    if (transport) {
        add(*transport);
    }
    for (const auto& existing : items_) {
        add(*existing);
    }
    if (item) {
        add(*item);
    }
    return total;
}

void Park::validateNewInventory(const IInventory& object) const {
    for (const auto* existing : inventory()) {
        if (existing->inventoryNumber() == object.inventoryNumber()) {
            throw std::invalid_argument("Inventory number is already in use");
        }
    }
    const double energy = energyOf(object);
    if (!std::isfinite(energy) || energy < 0) {
        throw std::invalid_argument("Daily energy must be finite and non-negative");
    }
}

bool Park::addTransport(std::unique_ptr<Transport> transport) {
    if (!transport) {
        throw std::invalid_argument("Transport must not be null");
    }
    validateNewInventory(*transport);
    if (serviceCenter_.inspect(*transport) != InspectResult::Accepted) {
        return false;
    }
    totalEnergyWith(transport.get());
    transports_.push_back(std::move(transport));
    return true;
}

void Park::addItem(std::unique_ptr<Item> item) {
    if (!item) {
        throw std::invalid_argument("Item must not be null");
    }
    validateNewInventory(*item);
    totalEnergyWith(nullptr, item.get());
    items_.push_back(std::move(item));
}

ParkReport Park::report() const {
    ParkReport result;
    result.transportCount = transports_.size();
    result.itemCount = items_.size();
    result.totalCount = result.transportCount + result.itemCount;
    result.dailyEnergyKWh = totalEnergyWith();
    return result;
}

std::vector<const Transport*> Park::beginners() const {
    std::vector<const Transport*> result;
    for (const auto& transport : transports_) {
        if (transport->simple_num() >= 6) {
            result.push_back(transport.get());
        }
    }
    return result;
}

std::vector<const IInventory*> Park::inventory() const {
    std::vector<const IInventory*> result;
    result.reserve(transports_.size() + items_.size());
    for (const auto& transport : transports_) {
        result.push_back(transport.get());
    }
    for (const auto& item : items_) {
        result.push_back(item.get());
    }
    return result;
}
