#include "park.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>

TEST(ParkTest, OverflowingEnergyDoesNotChangeParkAndTechnicalRejectionStillWins) {
    ServiceCenter center;
    Park park(center);
    const double maximum = std::numeric_limits<double>::max();
    park.addItem(std::make_unique<ChargingStation>(1, maximum));
    EXPECT_THROW(park.addItem(std::make_unique<ChargingStation>(2, maximum)),
                 std::invalid_argument);
    EXPECT_THROW(park.addTransport(std::make_unique<ElectricScooter>(
                     3, 6, 100, maximum)), std::invalid_argument);
    EXPECT_FALSE(park.addTransport(std::make_unique<ElectricScooter>(
                     4, 6, 69, maximum)));
    EXPECT_EQ(park.report().totalCount, 1U);
    EXPECT_TRUE(std::isfinite(park.report().dailyEnergyKWh));
    EXPECT_DOUBLE_EQ(park.report().dailyEnergyKWh, maximum);
}
