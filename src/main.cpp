#include "networking.hpp"

#include <fmt/core.h>
#include <stdexcept>

using networking::Curl;

int main()
{
    Curl curl;

    try {
        const std::string output = curl.create_message("What is 3 + 5?");
        fmt::print("{}\n", output);
    } catch (const std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
