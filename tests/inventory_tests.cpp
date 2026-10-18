#include "inventory.hpp"
#include "items.hpp"
#include "transport.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(InventoryNumberTest, StoresValue) {
    const InventoryNumber number(67);

    EXPECT_EQ(number.value(), 67);
    EXPECT_NO_THROW(InventoryNumber(1));
    EXPECT_THROW(InventoryNumber(0), std::invalid_argument);
    EXPECT_THROW(InventoryNumber(-1), std::invalid_argument);
    
    EXPECT_LT(InventoryNumber(1), InventoryNumber(2));
    EXPECT_EQ(InventoryNumber(7), InventoryNumber(7));
    EXPECT_FALSE(InventoryNumber(7) < InventoryNumber(7));
}

TEST(InventoryTest, NamesAndNumbersAreAvailableThroughCommonInterface) {
    const Bicycle bicycle(101, 6, 20);
    const ElectricScooter scooter(102, 5, 80, 2.5);
    const Helmet helmet(103);
    const ChargingStation station(104, 3.5);
    const IInventory* objects[] = {&bicycle, &scooter, &helmet, &station};
    const std::string_view names[] = {
        "Bicycle", "Electric Scooter", "Helmet", "Charging Station"
    };

    for (int index = 0; index < 4; ++index) {
        EXPECT_EQ(objects[index]->inventoryNumber().value(), 101 + index);
        EXPECT_EQ(objects[index]->name(), names[index]);
    }
    const IEnergyUser& scooterEnergy = scooter;
    const IEnergyUser& stationEnergy = station;
    EXPECT_DOUBLE_EQ(scooterEnergy.dailyEnergyKWh(), 2.5);
    EXPECT_DOUBLE_EQ(stationEnergy.dailyEnergyKWh(), 3.5);
    EXPECT_EQ(dynamic_cast<const IEnergyUser*>(objects[0]), nullptr);
    EXPECT_EQ(dynamic_cast<const IEnergyUser*>(objects[2]), nullptr);
}

TEST(InventoryTest, ModelConstructorsRejectNonPositiveIds) {
    for (int id : {0, -1}) {
        SCOPED_TRACE(id);
        EXPECT_THROW(Bicycle(id, 6, 20), std::invalid_argument);
        EXPECT_THROW(ElectricScooter(id, 6, 80, 2.5), std::invalid_argument);
        EXPECT_THROW((void)Helmet{id}, std::invalid_argument);
        EXPECT_THROW(ChargingStation(id, 3.5), std::invalid_argument);
    }
}
