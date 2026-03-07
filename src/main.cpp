#include "api.hpp"

#include <fmt/core.h>
#include <stdexcept>

using api::Curl;

void create_message()
{
    Curl curl;
    const std::string output = curl.create_message("What is 3 + 5?");
    fmt::print("{}\n", output);
}

int main()
{
    try {
        create_message();
    } catch (const std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
