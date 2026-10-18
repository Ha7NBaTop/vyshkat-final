#include "transport.hpp"
#include "items.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

TEST(BicycleTest, StoresCharacteristics) {
    const Bicycle bicycle(17, 5, 20);

    EXPECT_EQ(bicycle.simple_num(), 5);
    EXPECT_EQ(bicycle.brakePadWearPercent(), 20);
    EXPECT_THROW(Bicycle(1, 5, -1), std::invalid_argument);
    EXPECT_THROW(Bicycle(1, -1, 20), std::invalid_argument);
    EXPECT_THROW(Bicycle(1, 0, 20), std::invalid_argument);
    EXPECT_THROW(Bicycle(1, 1, 101), std::invalid_argument);
    EXPECT_THROW(Bicycle(1, 11, 20), std::invalid_argument);
    EXPECT_NO_THROW(Bicycle(1, 1, 20));
    EXPECT_NO_THROW(Bicycle(1, 10, 20));
    EXPECT_NO_THROW(Bicycle(1, 5, 100));
    EXPECT_NO_THROW(Bicycle(1, 5, 0));
}

TEST(ElectricScooterTest, StoresCharacteristics) {
    const ElectricScooter scooter(18, 10, 10, 2.5);

    EXPECT_EQ(scooter.simple_num(), 10);
    EXPECT_EQ(scooter.batteryHealthPercent(), 10);
    EXPECT_THROW(ElectricScooter(1, 5, -1, 1), std::invalid_argument);
    EXPECT_THROW(ElectricScooter(1, 1, 101, 1), std::invalid_argument);
    EXPECT_THROW(ElectricScooter(1, 0, 80, 1), std::invalid_argument);
    EXPECT_THROW(ElectricScooter(1, 11, 80, 1), std::invalid_argument);
    EXPECT_NO_THROW(ElectricScooter(1, 1, 100, 1));
    EXPECT_NO_THROW(ElectricScooter(1, 10, 0, 1));
}

TEST(EnergyUserTest, AcceptsZeroAndPositiveDailyConsumption) {
    const ElectricScooter idleScooter(1, 5, 80, 0);
    const ElectricScooter workingScooter(2, 5, 80, 2.5);
    const ChargingStation idleStation(3, 0);
    const ChargingStation workingStation(4, 12.75);

    EXPECT_DOUBLE_EQ(idleScooter.dailyEnergyKWh(), 0);
    EXPECT_DOUBLE_EQ(workingScooter.dailyEnergyKWh(), 2.5);
    EXPECT_DOUBLE_EQ(idleStation.dailyEnergyKWh(), 0);
    EXPECT_DOUBLE_EQ(workingStation.dailyEnergyKWh(), 12.75);
}

TEST(EnergyUserTest, RejectsNegativeAndNonFiniteDailyConsumption) {
    const double invalidValues[] = {
        -0.1,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
    };
    for (const double energy : invalidValues) {
        SCOPED_TRACE(energy);
        EXPECT_THROW(ElectricScooter(1, 5, 80, energy),
                     std::invalid_argument);
        EXPECT_THROW(ChargingStation(2, energy), std::invalid_argument);
    }
}
