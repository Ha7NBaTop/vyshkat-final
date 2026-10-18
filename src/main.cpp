#include "console_app.hpp"
#include <di.hpp>

#include <iostream>
#include <memory>

int main() {
    namespace di = boost::di;
    // Composition Root: singleton scopes keep dependencies alive through run().
    const auto injector = di::make_injector(
        di::bind<IServiceCenter>.to<ServiceCenter>().in(di::singleton),
        di::bind<Park>.to<Park>().in(di::singleton),
        di::bind<std::istream>.to(std::cin),
        di::bind<std::ostream>.to(std::cout));
    const auto app = injector.create<std::unique_ptr<ConsoleApp>>();
    app->run();
}
