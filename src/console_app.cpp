#include "console_app.hpp"

#include <iomanip>
#include <istream>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
struct EndOfInput {};
}

ConsoleApp::ConsoleApp(Park& park, std::istream& input, std::ostream& output)
    : park_(park), input_(input), output_(output) {}

template <class T>
T ConsoleApp::readNumber(std::string_view prompt) {
    for (;;) {
        output_ << prompt << std::flush;
        std::string line;
        if (!std::getline(input_, line)) {
            throw EndOfInput{};
        }
        std::istringstream parser(line);
        parser.imbue(std::locale::classic());
        T value{};
        if ((parser >> value) && (parser >> std::ws).eof()) {
            return value;
        }
        output_ << "Нужно одно число в допустимом диапазоне; дробная часть через точку.\n";
    }
}

void ConsoleApp::printAdmission(bool accepted) {
    output_ << (accepted ? "Техника принята в парк.\n"
                         : "Техосмотр: отказ. Техника не добавлена.\n");
}

void ConsoleApp::addBicycle() {
    const InventoryNumber number(readNumber<int>("Инвентарный номер (>0): "));
    const int simplicity = readNumber<int>("Простота (1-10): ");
    const int wear = readNumber<int>("Износ колодок (0-100%): ");
    printAdmission(park_.addTransport(std::make_unique<Bicycle>(number.value(), simplicity, wear)));
}

void ConsoleApp::addScooter() {
    const InventoryNumber number(readNumber<int>("Инвентарный номер (>0): "));
    const int simplicity = readNumber<int>("Простота (1-10): ");
    const int battery = readNumber<int>("Здоровье батареи (0-100%): ");
    const double energy = readNumber<double>("Суточная энергия (кВт·ч, >=0): ");
    printAdmission(park_.addTransport(
        std::make_unique<ElectricScooter>(number.value(), simplicity, battery, energy)));
}

void ConsoleApp::addHelmet() {
    const InventoryNumber number(readNumber<int>("Инвентарный номер (>0): "));
    park_.addItem(std::make_unique<Helmet>(number.value()));
    output_ << "Шлем добавлен на баланс.\n";
}

void ConsoleApp::addStation() {
    const InventoryNumber number(readNumber<int>("Инвентарный номер (>0): "));
    const double energy = readNumber<double>("Суточная энергия (кВт·ч, >=0): ");
    park_.addItem(std::make_unique<ChargingStation>(number.value(), energy));
    output_ << "Зарядная станция добавлена на баланс.\n";
}

void ConsoleApp::printReport() {
    const auto report = park_.report();
    output_ << "Транспорт: " << report.transportCount << "\nВещи: " << report.itemCount
            << "\nВсего на балансе: " << report.totalCount << '\n'
            << "Суточная энергия (транспорт + вещи): " << std::fixed
            << std::setprecision(2) << report.dailyEnergyKWh << " кВт·ч\n";
}

void ConsoleApp::printBeginners() {
    const auto objects = park_.beginners();
    output_ << "Принятая прокатная техника для новичков (простота >=6):\n";
    if (objects.empty()) {
        output_ << "Список пуст.\n";
    }
    for (const auto* object : objects) {
        output_ << object->inventoryNumber().value() << " | " << object->name()
                << " | простота " << object->simple_num() << '\n';
    }
}

void ConsoleApp::printInventory() {
    const auto objects = park_.inventory();
    output_ << "Инвентарь: номер | наименование\n";
    if (objects.empty()) {
        output_ << "Список пуст.\n";
    }
    for (const auto* object : objects) {
        output_ << object->inventoryNumber().value() << " | " << object->name() << '\n';
    }
}

void ConsoleApp::demonstrate() {
    // Check the entire reserved range before making any changes.
    for (const auto* object : park_.inventory()) {
        const int number = object->inventoryNumber().value();
        if (number >= 1001 && number <= 1006) {
            throw std::invalid_argument("Demo needs free inventory numbers 1001-1006");
        }
    }
    output_ << "Демонстрация: 2 приёма, 2 отказа, 2 вещи.\n";
    printAdmission(park_.addTransport(std::make_unique<Bicycle>(1001, 6, 30)));
    printAdmission(park_.addTransport(std::make_unique<ElectricScooter>(
        1002, 5, 70, 2.0)));
    printAdmission(park_.addTransport(std::make_unique<Bicycle>(1003, 10, 31)));
    printAdmission(park_.addTransport(std::make_unique<ElectricScooter>(
        1004, 10, 69, 50.0)));
    park_.addItem(std::make_unique<Helmet>(1005));
    park_.addItem(std::make_unique<ChargingStation>(1006, 3.0));
    printReport();
    printBeginners();
    printInventory();
}

void ConsoleApp::run() {
    output_ << "ВышКат: учебный учёт парка\n";
    try {
        for (;;) {
            output_ << "\n1 - велосипед; 2 - электросамокат; 3 - шлем; 4 - зарядная станция\n"
                       "5 - отчёт; 6 - техника для новичков; 7 - инвентарь; 8 - демонстрация; 0 - выход\n";
            const int choice = readNumber<int>("Выбор: ");
            try {
                switch (choice) {
                case 0: output_ << "До свидания.\n"; return;
                case 1: addBicycle(); break;
                case 2: addScooter(); break;
                case 3: addHelmet(); break;
                case 4: addStation(); break;
                case 5: printReport(); break;
                case 6: printBeginners(); break;
                case 7: printInventory(); break;
                case 8: demonstrate(); break;
                default: output_ << "Неизвестный пункт меню.\n"; break;
                }
            } catch (const std::invalid_argument& error) {
                output_ << "Ошибка: " << error.what() << ". Операция отменена.\n";
            }
        }
    } catch (const EndOfInput&) {
        output_ << "\nВвод завершён (EOF). До свидания.\n";
    }
}
