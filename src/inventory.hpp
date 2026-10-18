#pragma once
#include <compare>
#include <string_view>

class InventoryNumber{
    public:
        ~InventoryNumber() = default;
        auto operator<=>(const InventoryNumber&) const = default;
        explicit InventoryNumber(int value);
        int value() const;
    private:
        int value_;
};

class IInventory {
public:
    virtual ~IInventory() = default;
    virtual InventoryNumber inventoryNumber() const = 0;
    virtual std::string_view name() const = 0;
};
