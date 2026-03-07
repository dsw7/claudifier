#include "command_run.hpp"

#include <fmt/core.h>
#include <stdexcept>
#include <string>

void print_help_messages()
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

    const std::string command = argv[1];

    if (command == "-h" or command == "--help") {
        print_help_messages();
        return EXIT_SUCCESS;
    }

    try {
        if (command == "run") {
            commands::command_run();
        } else {
            throw std::runtime_error("Received unknown command. Re-run with -h or --help");
        }
    } catch (const std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
