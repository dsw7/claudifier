#include "command_chat.hpp"
#include "command_costs.hpp"
#include "command_models.hpp"
#include "command_run.hpp"
#include "command_test.hpp"
#include "configs.hpp"

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
  run            Run a stateless query against an appropriate model
  models         Get list of available models
  chat           Participate in a stateful conversation with an appropriate model

Administrative commands:
  costs          Get cost report

Try 'claudifier <subcommand> [-h | --help]' for subcommand specific help.
)";

    fmt::print("{}", messages);
}

void print_build_information()
{
    nlohmann::json data;

    data["build_date"] = BUILD_DATE;
    data["cmake_build_type"] = CMAKE_BUILD_TYPE;
    data["version"] = PROJECT_VERSION;

#ifdef TESTING_ENABLED
    data["build_type"] = "Testing";
#else
    data["build_type"] = "Production";
#endif

    fmt::print("{}\n", data.dump(2));
}

Configs load_configs()
{
    return Configs();
}

int run_default_command(const int argc, char **argv)
{
    try {
        Configs configs = load_configs();
        commands::command_chat(argc, argv, configs.configs_chat);
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

void run_command(const int argc, char **argv, const std::string &command)
{
    Configs configs = load_configs();

    if (command == "run") {
        commands::command_run(argc, argv, configs.configs_run);
    } else if (command == "models") {
        commands::command_models(argc, argv);
    } else if (command == "chat") {
        commands::command_chat(argc, argv, configs.configs_chat);
    } else if (command == "costs") {
        commands::command_costs(argc, argv);
    } else if (command == "test") {
        commands::command_test(argc, argv);
    } else {
        throw std::runtime_error("Received unknown command. Re-run with -h or --help");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        return run_default_command(argc, argv);
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
