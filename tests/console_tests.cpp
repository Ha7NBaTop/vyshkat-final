#include "console_app.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

namespace {
std::string runConsole(const std::string& script) {
    ServiceCenter service;
    Park park(service);
    std::istringstream input(script);
    std::ostringstream output;
    ConsoleApp app(park, input, output);
    app.run();
    return output.str();
}
}

TEST(ConsoleTest, EmptyReportsAndExit) {
    const auto text = runConsole("5\n6\n7\n0\n");
    EXPECT_NE(text.find("Всего на балансе: 0"), std::string::npos);
    EXPECT_NE(text.find("0.00 кВт·ч"), std::string::npos);
    EXPECT_NE(text.find("Список пуст."), std::string::npos);
    EXPECT_NE(text.find("До свидания."), std::string::npos);
}

TEST(ConsoleTest, DemoShowsAcceptedRejectedAndAllReports) {
    const auto text = runConsole("8\n0\n");
    EXPECT_NE(text.find("Техника принята в парк."), std::string::npos);
    EXPECT_NE(text.find("Техосмотр: отказ."), std::string::npos);
    EXPECT_NE(text.find("Всего на балансе: 4"), std::string::npos);
    EXPECT_NE(text.find("5.00 кВт·ч"), std::string::npos);
    EXPECT_NE(text.find("1001 | Bicycle | простота 6"), std::string::npos);
    EXPECT_EQ(text.find("1002 | Electric Scooter | простота"), std::string::npos);
    EXPECT_EQ(text.find("1003 |"), std::string::npos);
    EXPECT_EQ(text.find("1004 |"), std::string::npos);
}

TEST(ConsoleTest, ManualAdmissionsAndItemsAreReported) {
    const auto text = runConsole(
        "1\n1\n6\n30\n2\n2\n5\n70\n2.5\n3\n3\n4\n4\n3.25\n5\n7\n0\n");
    EXPECT_NE(text.find("Всего на балансе: 4"), std::string::npos);
    EXPECT_NE(text.find("5.75 кВт·ч"), std::string::npos);
    EXPECT_NE(text.find("Шлем добавлен"), std::string::npos);
    EXPECT_NE(text.find("Зарядная станция добавлена"), std::string::npos);
}

TEST(ConsoleTest, RejectsBadInputAndRecovers) {
    const auto text = runConsole(
        "oops\n\n2junk\n99999999999999999999999\n42\n"
        "3\n0\n1\n1\n11\n20\n4\n2\n-1\n"
        "3\n10\n3\n10\n5\n0\n");
    EXPECT_NE(text.find("Нужно одно число"), std::string::npos);
    EXPECT_NE(text.find("Неизвестный пункт"), std::string::npos);
    EXPECT_NE(text.find("Ошибка:"), std::string::npos);
    EXPECT_NE(text.find("Всего на балансе: 1"), std::string::npos);
    EXPECT_NE(text.find("До свидания."), std::string::npos);
}

TEST(ConsoleTest, EofAtMenuOrHalfwayThroughAdmissionStopsCleanly) {
    EXPECT_NE(runConsole("").find("EOF"), std::string::npos);
    ServiceCenter service;
    Park park(service);
    std::istringstream input("2\n1\n6\n");
    std::ostringstream output;
    ConsoleApp app(park, input, output);
    app.run();
    EXPECT_EQ(park.report().totalCount, 0U);
    EXPECT_NE(output.str().find("EOF"), std::string::npos);
}

TEST(ConsoleTest, RepeatedDemoCancelsBeforeChangingPark) {
    const auto text = runConsole("8\n8\n5\n0\n");
    EXPECT_NE(text.find("Demo needs free"), std::string::npos);
    EXPECT_EQ(text.find("Всего на балансе: 8"), std::string::npos);
}
