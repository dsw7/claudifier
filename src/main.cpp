#include "command_run.hpp"

#include <fmt/core.h>
#include <stdexcept>
#include <string>

void print_help_messages()
{
    fmt::print("-- Claudifier | v{}\n", PROJECT_VERSION);
    fmt::print("-- Copyright (C) 2026-{} by David Weber\n", CURRENT_YEAR);

    const std::string messages = R"(-- Site: https://github.com/dsw7/Claudifier

A command line Claude API toolkit.

Usage:
  cl [OPTIONS] COMMAND [ARGS]...

Options:
  -h, --help     Print help information and exit
  -v, --version  Print version and exit

Commands:
  run            Run a query against an appropriate model

Try 'cl <subcommand> [-h | --help]' for subcommand specific help.
)";

    fmt::print("{}\n", messages);
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
