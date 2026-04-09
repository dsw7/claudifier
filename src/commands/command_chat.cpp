#include "command_chat.hpp"

#include "query_messages_api.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

using api::CreateMessage;
using api::Err;
using api::MessagesOutput;

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
  -t, --temperature              Specify how random the response will be.
                                 The temperature is clamped between 0.0 and 1.0
)";

    fmt::print("{}\n", messages);
}

void print_special_commands_()
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Commands: \n");
    fmt::print("[");
    fmt::print(fg(colors::green), "q");
    fmt::print("]: Quit the current chat session\n[");
    fmt::print(fg(colors::green), "?");
    fmt::print("]: Print this list of commands\n");
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

void print_output_to_stdout_(const MessagesOutput &output)
{
    fmt::print(fg(colors::green), "{}\n\n", output.get_latest_text());
}

void print_usage_to_stdout_(const MessagesOutput &output)
{
    fmt::print(fmt::emphasis::bold, "Usage: \n");
    fmt::print("Input tokens: {}\n", output.input_tokens);
    fmt::print("Output tokens: {}\n", output.output_tokens);
}

bool break_conversation_on_condition_(const MessagesOutput &output)
{
    if (output.stop_reason == "end_turn") {
        return false;
    }

    fmt::print(fg(colors::yellow), "Stop reason: {}\n", output.stop_reason);

    if (output.stop_reason == "max_tokens") {
        fmt::print(fg(colors::red), "Cannot continue. The conversation has become fragmented due to token limitations.\n");
    }

    return true;
}

MessagesOutput run_query_(CreateMessage &input)
{
    std::expected<MessagesOutput, Err> output = input.query_api();

    if (output) {
        return output.value();
    }

    throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
}

void run_conversational_loop_(CreateMessage &input)
{
    MessagesOutput output;
    LoopControl loop_controller;

    print_special_commands_();

    while (true) {
        const std::string message = utils::read_input_from_stdin();
        loop_controller = parse_special_command_(message);

        if (loop_controller == LoopControl::BREAK) {
            break;
        } else if (loop_controller == LoopControl::CONTINUE) {
            continue;
        }

        input.append_user_message(message);
        output = run_query_(input);
        print_output_to_stdout_(output);
        print_usage_to_stdout_(output);
        if (break_conversation_on_condition_(output)) {
            break;
        }
        input.append_assistant_message(output.get_latest_text());
    }
}

} // namespace

namespace commands {

void command_chat(const int argc, char **argv)
{
    float temperature = 1.0f;
    int max_tokens = 1024;
    std::string model = "claude-3-haiku-20240307";

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "limit", required_argument, 0, 'l' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:l:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'm':
                model = optarg;
                break;
            case 'l':
                max_tokens = utils::string_to_int(optarg);
                break;
            case 't':
                temperature = utils::string_to_float(optarg);
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} chat [-h | --help] for more information", argv[0]));
        }
    };

    CreateMessage input(max_tokens, temperature, model);
    run_conversational_loop_(input);
}

} // namespace commands
