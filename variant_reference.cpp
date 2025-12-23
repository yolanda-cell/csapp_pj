#include <variant>
#include <string>
#include <iostream>

using Value = std::variant<long, std::string>;

int main() {
    Value a = std::string("hello");
    Value b = a; // deep copy of std::string

    std::visit([](auto&& x){ std::cout << x << "\n"; }, a);
    std::visit([](auto&& x){ std::cout << x << "\n"; }, b);

    // No manual destroy; no double free.
}