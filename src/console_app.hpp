#pragma once

#include "park.hpp"

#include <iosfwd>
#include <string_view>

class ConsoleApp {
public:
    ConsoleApp(Park& park, std::istream& input, std::ostream& output);
    void run();

private:
    template <class T> T readNumber(std::string_view prompt);
    void addBicycle();
    void addScooter();
    void addHelmet();
    void addStation();
    void printReport();
    void printBeginners();
    void printInventory();
    void demonstrate();
    void printAdmission(bool accepted);

    Park& park_;
    std::istream& input_;
    std::ostream& output_;
};
