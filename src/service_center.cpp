#include "service_center.hpp"

ServiceCenter::ServiceCenter() {
    registerRule<Bicycle>([](const Bicycle& bicycle) {
        return bicycle.brakePadWearPercent() <= 30
            ? InspectResult::Accepted : InspectResult::Rejected;
    });
    registerRule<ElectricScooter>([](const ElectricScooter& scooter) {
        return scooter.batteryHealthPercent() >= 70
            ? InspectResult::Accepted : InspectResult::Rejected;
    });
}

InspectResult ServiceCenter::inspect(const Transport& transport) const {
    const auto rule = rules_.find(std::type_index(typeid(transport)));
    if (rule == rules_.end()) {
        return InspectResult::Rejected;
    }
    return rule->second(transport);
}
