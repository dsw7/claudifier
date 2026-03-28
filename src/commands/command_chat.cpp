#include "command_chat.hpp"

#include "command_utils.hpp"
#include "query_messages_api.hpp"

#include <array>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef TESTING_ENABLED
#include <fmt/color.h>
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;
constexpr fmt::terminal_color yellow = fmt::terminal_color::bright_yellow;
constexpr fmt::terminal_color red = fmt::terminal_color::bright_red;
#endif

namespace {

using api::MessagesInput;

void print_help_messages_()
{
    const std::string messages = R"(Participate in a multi-turn conversation with an LLM. Note that this
command preserves context between API calls and therefore will be more
token heavy.

Usage:
  claudifier chat [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -l, --limit=LIMIT              Specify token limit
)";

    fmt::print("{}\n", messages);
}

MessagesInput read_cli_(const int argc, char **argv)
{
    MessagesInput model;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:l:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'm':
                model.set_llm_model(optarg);
                break;
            case 'l':
                model.set_max_tokens(utils::string_to_int(optarg));
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    return model;
}

// ----------------------------------------------------------------------------------------------------------

#ifndef TESTING_ENABLED
void print_special_commands_()
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Commands: \n");
    fmt::print("[");
    fmt::print(fg(green), "q");
    fmt::print("]: Quit the current chat session\n[");
    fmt::print(fg(green), "?");
    fmt::print("]: Print this list of commands\n");
}

void read_input_from_stdin_(std::string &input)
{
    utils::print_line();

    while (true) {
        fmt::print(fmt::emphasis::bold, "Input: ");
        std::getline(std::cin, input);

        if (input.empty()) {
            fmt::print("Input is empty. Try again.\n");
        } else {
            break;
        }
    }
}

enum class LoopControl {
    BREAK,
    CONTINUE,
    PROCEED,
};

LoopControl parse_special_command_(const std::string &special_command)
{
    if (special_command.size() != 1) {
        return LoopControl::PROCEED;
    }

    if (special_command == "q") {
        fmt::print("Aborting conversation.\n");
        return LoopControl::BREAK;
    }

    if (special_command == "?") {
        print_special_commands_();
        return LoopControl::CONTINUE;
    }

    fmt::print("Received invalid command: '{}'\n", special_command);
    return LoopControl::CONTINUE;
}

void print_output_to_stdout_(const MessagesResult &results)
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Output: ");
    fmt::print(fg(green), "{}\n", results.output);
}

void print_usage_to_stdout_(const MessagesResult &results)
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Usage: \n");
    fmt::print("Input tokens: {}\n", results.input_tokens);
    fmt::print("Output tokens: {}\n", results.output_tokens);
}

bool break_conversation_on_condition_(const MessagesResult &results)
{
    if (results.stop_reason == "end_turn") {
        return false;
    }

    fmt::print(fg(yellow), "Stop reason: {}\n", results.stop_reason);

    if (results.stop_reason == "max_tokens") {
        fmt::print(fg(red), "Cannot continue. The conversation has become fragmented due to token limitations.\n");
    }

    return true;
}
#endif

MessagesResult run_query_(const MessagesInput &model, api::CreateMessage &api_handle)
{
    std::expected<MessagesResult, Err> result = api_handle.query_api(model);

    if (result) {
        return result.value();
    }

    throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", result.error().errmsg));
}

} // namespace

namespace commands {

void command_chat(const int argc, char **argv)
{
    MessagesInput model = read_cli_(argc, argv);
    api::CreateMessage api_handle;
    MessagesResult result;

#ifdef TESTING_ENABLED
    // mock conversational turns without interactivity for testing
    const std::array<std::string, 2> messages = {
        "If a = 2, b = 3, and c = a + b, then what is c?",
        "What is c + 5? Return just the value",
    };

    for (const auto &input: messages) {
        model.append_user_message(input);
        result = run_query_(model, api_handle);
        model.append_assistant_message(result.output);
    }

    fmt::print("{}\n", result.output);
#else
    std::string input;
    print_special_commands_();
    LoopControl loop_controller;

    while (true) {
        read_input_from_stdin_(input);
        loop_controller = parse_special_command_(input);

        if (loop_controller == LoopControl::BREAK) {
            break;
        } else if (loop_controller == LoopControl::CONTINUE) {
            continue;
        }

        model.append_user_message(input);
        result = run_query_(model, api_handle);
        print_output_to_stdout_(result);
        print_usage_to_stdout_(result);
        if (break_conversation_on_condition_(result)) {
            break;
        }
        model.append_assistant_message(result.output);
    }
#endif
}

} // namespace commands
