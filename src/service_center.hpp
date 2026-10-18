#pragma once

#include "transport.hpp"

#include <functional>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <utility>

enum class InspectResult {
    Accepted,
    Rejected
};

class IServiceCenter {
public:
    virtual ~IServiceCenter() = default;
    virtual InspectResult inspect(const Transport& transport) const = 0;
};

class ServiceCenter : public IServiceCenter {
public:
    ServiceCenter();
    InspectResult inspect(const Transport& transport) const override;

    // Each rule handles exactly T. Derived types need their own registration.
    template <class T>
    void registerRule(std::function<InspectResult(const T&)> rule) {
        static_assert(std::is_base_of_v<Transport, T>, "T must derive from Transport");
        if (!rule) {
            throw std::invalid_argument("Inspection rule must not be empty");
        }
        rules_.insert_or_assign(std::type_index(typeid(T)),
            [rule = std::move(rule)](const Transport& transport) {
                return rule(static_cast<const T&>(transport));
            });
    }

private:
    std::map<std::type_index, std::function<InspectResult(const Transport&)>> rules_;
};
