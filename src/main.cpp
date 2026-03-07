#include "api.hpp"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>
#include <string>

using api::Curl;

void create_message()
{
    Curl curl;
    const std::string model = "claude-opus-4-6";
    const auto result = curl.create_message("What is 3 + 5?", model);

    if (result) {
        fmt::print("{}\n", result->output);
    } else {
        throw std::runtime_error(result.error().errmsg);
    }
}

void print_help()
{
    fmt::print("Usage: program [option]\n");
    fmt::print("Options:\n");
    fmt::print("  -h, --help   Show help menu\n");
    fmt::print("  run          Run the create_message function\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fmt::print(stderr, "Invalid argument. Try '-h' or '--help' for more information.\n");
        return EXIT_FAILURE;
    }

    std::string arg = argv[1];

    if (arg == "-h" || arg == "--help") {
        print_help();
        return EXIT_SUCCESS;
    } else if (arg == "run") {
        try {
            create_message();
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "{}\n", e.what());
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } else {
        fmt::print(stderr, "Unknown command '{}'. Try '-h' or '--help' for more information.\n", arg);
        return EXIT_FAILURE;
    }
}
