#include "networking.hpp"

#include <iostream>
#include <stdexcept>

using networking::Curl;

int main()
{
    Curl curl;

    try {
        const std::string output = curl.create_message("What is 3 + 5?");
        std::cout << output << '\n';
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
