#include "api.hpp"

#include <fmt/core.h>
#include <stdexcept>

using api::Curl;

void create_message()
{
    Curl curl;
    const auto output = curl.create_message("What is 3 + 5?");

    if (output) {
        fmt::print("{}\n", output->message);
    } else {
        throw std::runtime_error(output.error().message);
    }
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
