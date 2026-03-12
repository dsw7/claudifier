#include "command_models.hpp"
#include "command_run.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

#ifndef TESTING_ENABLED
constexpr fmt::terminal_color red = fmt::terminal_color::bright_red;
#endif

void print_help_messages()
{
    fmt::print("-- Claudifier | v{}\n", PROJECT_VERSION);
    fmt::print("-- Copyright (C) 2026-{} by David Weber\n", CURRENT_YEAR);

    const std::string messages = R"(-- Site: https://github.com/dsw7/Claudifier

A command line Claude API toolkit.

Usage:
  claudifier [OPTIONS] COMMAND [ARGS]...

Options:
  -h, --help     Print help information and exit
  -v, --version  Print version and exit

Commands:
  run            Run a query against an appropriate model
  models         Get list of available models

Try 'claudifier <subcommand> [-h | --help]' for subcommand specific help.
)";

    fmt::print("{}\n", messages);
}

void print_build_information()
{
    nlohmann::json data;

    data["build_date"] = BUILD_DATE;
    data["version"] = PROJECT_VERSION;

#ifdef TESTING_ENABLED
    data["build_type"] = "Testing";
#else
    data["build_type"] = "Production";
#endif

    fmt::print("{}\n", data.dump(2));
}

void run_command(const int argc, char **argv, const std::string &command)
{
    if (command == "run") {
        commands::command_run(argc, argv);
    } else if (command == "models") {
        commands::command_models(argc, argv);
    } else {
        throw std::runtime_error("Received unknown command. Re-run with -h or --help");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        commands::command_run(argc, argv);
        return EXIT_SUCCESS;
    }

    const std::string command = argv[1];

    if (command == "-h" or command == "--help") {
        print_help_messages();
        return EXIT_SUCCESS;
    }

    if (command == "-v" or command == "--version") {
        print_build_information();
        return EXIT_SUCCESS;
    }

    try {
        run_command(argc, argv, command);
    } catch (const std::runtime_error &e) {
#ifdef TESTING_ENABLED
        fmt::print(stderr, "{}\n", e.what());
#else
        fmt::print(stderr, fg(red), "{}\n", e.what());
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
