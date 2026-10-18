#include "service_center.hpp"

#include <gtest/gtest.h>
#include <functional>
#include <stdexcept>

namespace {

class CargoBicycle : public Bicycle {
public:
    using Bicycle::Bicycle;
};

class CustomTransport : public Transport {
public:
    CustomTransport(int number, bool ready)
        : Transport(number, 6), ready_(ready) {}

    std::string_view name() const override { return "Custom Transport"; }
    bool ready() const { return ready_; }

private:
    bool ready_;
};

}

TEST(ServiceCenterTest, InspectionUsesTechnicalRanges) {
    const ServiceCenter center;

    EXPECT_EQ(center.inspect(Bicycle(1, 1, 0)), InspectResult::Accepted);
    EXPECT_EQ(center.inspect(Bicycle(1, 1, 31)), InspectResult::Rejected);
    EXPECT_EQ(center.inspect(Bicycle(1, 1, 30)), InspectResult::Accepted);
    EXPECT_EQ(center.inspect(Bicycle(1, 1, 100)), InspectResult::Rejected);

    EXPECT_EQ(center.inspect(ElectricScooter(1, 1, 70, 1)),
              InspectResult::Accepted);
    EXPECT_EQ(center.inspect(ElectricScooter(1, 1, 0, 1)),
              InspectResult::Rejected);
    EXPECT_EQ(center.inspect(ElectricScooter(1, 1, 100, 1)),
              InspectResult::Accepted);
    EXPECT_EQ(center.inspect(ElectricScooter(1, 1, 69, 1)),
              InspectResult::Rejected);
}

TEST(ServiceCenterTest, TechnicalInspectionDoesNotDependOnSimplicity) {
    const ServiceCenter center;
    EXPECT_EQ(center.inspect(Bicycle(1, 1, 30)),
              center.inspect(Bicycle(2, 10, 30)));
    EXPECT_EQ(center.inspect(ElectricScooter(3, 1, 69, 0)),
              center.inspect(ElectricScooter(4, 10, 69, 100)));
}

TEST(ServiceCenterTest, UnknownTypesNeedTheirOwnInspectionRule) {
    ServiceCenter center;
    const CustomTransport ready(1, true);
    const CustomTransport unready(2, false);

    EXPECT_EQ(center.inspect(ready), InspectResult::Rejected);
    center.registerRule<CustomTransport>([](const CustomTransport& transport) {
        return transport.ready() ? InspectResult::Accepted : InspectResult::Rejected;
    });

    EXPECT_EQ(center.inspect(ready), InspectResult::Accepted);
    EXPECT_EQ(center.inspect(unready), InspectResult::Rejected);
    EXPECT_EQ(center.inspect(Bicycle(3, 5, 30)), InspectResult::Accepted);
    EXPECT_EQ(center.inspect(ElectricScooter(4, 5, 69, 1)),
              InspectResult::Rejected);
}

TEST(ServiceCenterTest, DerivedTypesDoNotSilentlyInheritBaseInspectionRule) {
    ServiceCenter center;
    const CargoBicycle cargo(1, 7, 0);

    EXPECT_EQ(center.inspect(cargo), InspectResult::Rejected);
    center.registerRule<CargoBicycle>([](const CargoBicycle& bicycle) {
        return bicycle.brakePadWearPercent() <= 10
            ? InspectResult::Accepted : InspectResult::Rejected;
    });
    EXPECT_EQ(center.inspect(cargo), InspectResult::Accepted);
    EXPECT_EQ(center.inspect(CargoBicycle(2, 7, 11)), InspectResult::Rejected);
    EXPECT_EQ(center.inspect(Bicycle(3, 7, 11)), InspectResult::Accepted);
}

TEST(ServiceCenterTest, RuleCanBeReplacedAndEmptyRuleIsRejected) {
    ServiceCenter center;
    const Bicycle bicycle(1, 5, 0);

    center.registerRule<Bicycle>([](const Bicycle&) { return InspectResult::Rejected; });
    EXPECT_EQ(center.inspect(bicycle), InspectResult::Rejected);

    const std::function<InspectResult(const Bicycle&)> emptyRule;
    EXPECT_THROW(center.registerRule<Bicycle>(emptyRule), std::invalid_argument);
    EXPECT_EQ(center.inspect(bicycle), InspectResult::Rejected);
}
