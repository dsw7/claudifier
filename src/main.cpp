#include "api.hpp"

#include <fmt/core.h>
#include <stdexcept>

using api::Curl;

void create_message()
{
    Curl curl;
    const auto result = curl.create_message("What is 3 + 5?");

    if (result) {
        fmt::print("{}\n", result->output);
    } else {
        throw std::runtime_error(result.error().errmsg);
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
