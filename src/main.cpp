#include "command_run.hpp"

#include <fmt/core.h>
#include <stdexcept>
#include <string>

void print_help()
{
    fmt::print("Usage: program [option]\n");
    fmt::print("Options:\n");
    fmt::print("  -h, --help   Show help menu\n");
    fmt::print("  run          Run the create_message function\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        commands::command_run();
        return EXIT_SUCCESS;
    }

    const std::string arg = argv[1];

    if (arg == "-h" || arg == "--help") {
        print_help();
        return EXIT_SUCCESS;
    } else if (arg == "run") {
        try {
            commands::command_run();
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
