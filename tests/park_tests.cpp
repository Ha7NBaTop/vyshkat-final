#include "park.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace {

class FakeServiceCenter : public IServiceCenter {
public:
    explicit FakeServiceCenter(InspectResult result) : result(result) {}

    InspectResult inspect(const Transport& transport) const override {
        ++calls;
        lastInspectedNumber = transport.inventoryNumber().value();
        return result;
    }

    InspectResult result;
    mutable int calls = 0;
    mutable int lastInspectedNumber = 0;
};

class CustomElectricTransport : public Transport, public IEnergyUser {
public:
    CustomElectricTransport(int number, int simplicity, bool ready, double energy)
        : Transport(number, simplicity), ready_(ready), energy_(energy) {}

    std::string_view name() const override { return "Electric Cargo Cart"; }
    double dailyEnergyKWh() const override { return energy_; }
    bool ready() const { return ready_; }

private:
    bool ready_;
    double energy_;
};

class CustomElectricItem : public Item, public IEnergyUser {
public:
    CustomElectricItem(int number, double energy) : Item(number), energy_(energy) {}

    std::string_view name() const override { return "Electric Item"; }
    double dailyEnergyKWh() const override { return energy_; }

private:
    double energy_;
};

std::map<int, std::string> inventoryRecords(const Park& park) {
    std::map<int, std::string> result;
    for (const auto* object : park.inventory()) {
        result.emplace(object->inventoryNumber().value(), object->name());
    }
    return result;
}

std::set<int> beginnerNumbers(const Park& park) {
    std::set<int> result;
    for (const auto* transport : park.beginners()) {
        result.insert(transport->inventoryNumber().value());
    }
    return result;
}

void expectReport(const Park& park, std::size_t transports, std::size_t items, double energy) {
    const auto report = park.report();
    EXPECT_EQ(report.transportCount, transports);
    EXPECT_EQ(report.itemCount, items);
    EXPECT_EQ(report.totalCount, transports + items);
    EXPECT_DOUBLE_EQ(report.dailyEnergyKWh, energy);
}

} // namespace

TEST(ParkTest, EmptyParkHasEmptyViewsAndZeroReport) {
    FakeServiceCenter service(InspectResult::Accepted);
    const Park park(service);

    expectReport(park, 0, 0, 0);
    EXPECT_TRUE(park.inventory().empty());
    EXPECT_TRUE(park.beginners().empty());
    EXPECT_EQ(service.calls, 0);
}

TEST(ParkTest, InjectedServiceCanAcceptTechnicallyUnsuitableTransport) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);

    EXPECT_TRUE(park.addTransport(std::make_unique<Bicycle>(1, 6, 100)));
    EXPECT_TRUE(park.addTransport(
        std::make_unique<ElectricScooter>(2, 7, 0, 4.5)));

    EXPECT_EQ(service.calls, 2);
    EXPECT_EQ(service.lastInspectedNumber, 2);
    expectReport(park, 2, 0, 4.5);
    EXPECT_EQ(beginnerNumbers(park), (std::set<int>{1, 2}));
}

TEST(ParkTest, RejectedTransportIsAbsentFromEveryViewAndReport) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(1, 5, 0)));
    park.addItem(std::make_unique<ChargingStation>(2, 3.5));
    const auto before = inventoryRecords(park);
    service.result = InspectResult::Rejected;

    EXPECT_FALSE(park.addTransport(
        std::make_unique<ElectricScooter>(3, 10, 100, 20)));

    EXPECT_EQ(service.calls, 2);
    EXPECT_EQ(service.lastInspectedNumber, 3);
    expectReport(park, 1, 1, 3.5);
    EXPECT_EQ(inventoryRecords(park), before);
    EXPECT_TRUE(park.beginners().empty());
}

TEST(ParkTest, BeginnersIncludeSimplicitySixButExcludeFive) {
    ServiceCenter service;
    Park park(service);
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(1, 5, 30)));
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(2, 6, 30)));
    ASSERT_TRUE(park.addTransport(
        std::make_unique<ElectricScooter>(3, 5, 70, 0)));
    ASSERT_TRUE(park.addTransport(
        std::make_unique<ElectricScooter>(4, 6, 70, 0)));
    park.addItem(std::make_unique<Helmet>(5));

    EXPECT_EQ(beginnerNumbers(park), (std::set<int>{2, 4}));
    EXPECT_EQ(park.beginners().size(), 2u);
    expectReport(park, 4, 1, 0);
}

TEST(ParkTest, MixedReportCountsAllObjectsAndOnlyEnergyUsersContributeEnergy) {
    ServiceCenter service;
    Park park(service);
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(10, 2, 0)));
    ASSERT_TRUE(park.addTransport(
        std::make_unique<ElectricScooter>(20, 6, 90, 2.25)));
    park.addItem(std::make_unique<Helmet>(30));
    park.addItem(std::make_unique<ChargingStation>(40, 12.5));

    expectReport(park, 2, 2, 14.75);
    const std::map<int, std::string> expected = {
        {10, "Bicycle"},
        {20, "Electric Scooter"},
        {30, "Helmet"},
        {40, "Charging Station"},
    };
    EXPECT_EQ(inventoryRecords(park), expected);
    EXPECT_EQ(park.inventory().size(), 4u);
    expectReport(park, 2, 2, 14.75);
}

TEST(ParkTest, DuplicateTransportNumberCannotChangePark) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(1, 5, 0)));

    EXPECT_THROW(park.addTransport(
        std::make_unique<ElectricScooter>(1, 10, 100, 100)),
        std::invalid_argument);

    expectReport(park, 1, 0, 0);
    EXPECT_EQ(park.inventory().size(), 1u);
    EXPECT_TRUE(park.beginners().empty());
}

TEST(ParkTest, InventoryNumbersAreUniqueAcrossBothCategories) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);
    ASSERT_TRUE(park.addTransport(std::make_unique<Bicycle>(1, 5, 0)));
    park.addItem(std::make_unique<ChargingStation>(2, 4.5));

    EXPECT_THROW(park.addItem(std::make_unique<Helmet>(1)),
                 std::invalid_argument);
    EXPECT_THROW(park.addItem(std::make_unique<Helmet>(2)),
                 std::invalid_argument);
    EXPECT_THROW(park.addTransport(
        std::make_unique<ElectricScooter>(2, 10, 100, 20)),
        std::invalid_argument);

    expectReport(park, 1, 1, 4.5);
    EXPECT_EQ(park.inventory().size(), 2u);
    EXPECT_TRUE(park.beginners().empty());
}

TEST(ParkTest, RejectedInventoryNumberCanBeReusedForAcceptedTransportOrItem) {
    FakeServiceCenter service(InspectResult::Rejected);
    Park park(service);
    EXPECT_FALSE(park.addTransport(std::make_unique<Bicycle>(1, 6, 0)));
    service.result = InspectResult::Accepted;
    EXPECT_TRUE(park.addTransport(
        std::make_unique<ElectricScooter>(1, 6, 80, 1.25)));
    service.result = InspectResult::Rejected;
    EXPECT_FALSE(park.addTransport(std::make_unique<Bicycle>(2, 6, 0)));
    EXPECT_NO_THROW(park.addItem(std::make_unique<Helmet>(2)));

    expectReport(park, 1, 1, 1.25);
    EXPECT_EQ(beginnerNumbers(park), (std::set<int>{1}));
    EXPECT_EQ(park.inventory().size(), 2u);
}

TEST(ParkTest, NullOwnershipArgumentsAreRejectedWithoutChangingPark) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);
    park.addItem(std::make_unique<ChargingStation>(1, 2));

    EXPECT_THROW(park.addTransport(nullptr), std::invalid_argument);
    EXPECT_THROW(park.addItem(nullptr), std::invalid_argument);

    expectReport(park, 0, 1, 2);
    EXPECT_EQ(park.inventory().size(), 1u);
    EXPECT_TRUE(park.beginners().empty());
    EXPECT_EQ(service.calls, 0);
}

TEST(ParkTest, NewTransportCanBeAddedThroughRegisteredRuleWithoutChangingPark) {
    ServiceCenter service;
    Park park(service);
    EXPECT_FALSE(park.addTransport(
        std::make_unique<CustomElectricTransport>(1, 6, true, 3.25)));
    service.registerRule<CustomElectricTransport>([](const CustomElectricTransport& cart) {
        return cart.ready() ? InspectResult::Accepted : InspectResult::Rejected;
    });

    EXPECT_TRUE(park.addTransport(
        std::make_unique<CustomElectricTransport>(1, 6, true, 3.25)));
    EXPECT_FALSE(park.addTransport(
        std::make_unique<CustomElectricTransport>(2, 10, false, 30)));
    park.addItem(std::make_unique<CustomElectricItem>(3, 0.75));

    expectReport(park, 1, 1, 4);
    EXPECT_EQ(beginnerNumbers(park), (std::set<int>{1}));
    EXPECT_EQ(inventoryRecords(park),
              (std::map<int, std::string>{{1, "Electric Cargo Cart"}, {3, "Electric Item"}}));
}

TEST(ParkTest, RejectsInvalidEnergyFromFutureTransportAndItemTypes) {
    FakeServiceCenter service(InspectResult::Accepted);
    Park park(service);
    const double invalidValues[] = {
        -1,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
    };
    for (const double energy : invalidValues) {
        SCOPED_TRACE(energy);
        EXPECT_THROW(park.addTransport(std::make_unique<CustomElectricTransport>(
            1, 6, true, energy)), std::invalid_argument);
        EXPECT_THROW(park.addItem(std::make_unique<CustomElectricItem>(
            2, energy)), std::invalid_argument);
    }

    EXPECT_TRUE(park.addTransport(
        std::make_unique<CustomElectricTransport>(1, 6, true, 0)));
    EXPECT_NO_THROW(park.addItem(std::make_unique<CustomElectricItem>(2, 1.5)));
    expectReport(park, 1, 1, 1.5);
    EXPECT_EQ(park.inventory().size(), 2u);
}
