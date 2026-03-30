#include "command_run.hpp"

#include "command_utils.hpp"
#include "query_messages_api.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#ifdef TESTING_ENABLED
#include <json.hpp>
#else
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;
#endif

namespace {

using api::CreateMessage;
using api::Err;
using api::MessagesInput;
using api::MessagesOutput;

void print_help_messages_()
{
    const std::string messages = R"(Create a message according to a prompt. Messaging using the run
command is stateless. The prompt can be read in interactively or via
command line argument.

Usage:
  claudifier run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -p, --prompt=PROMPT            Specify the prompt message
  -l, --limit=LIMIT              Specify token limit
  -r, --raw                      Print raw response from server
  -s, --system                   Provide a system prompt
  -t, --temperature              Specify how random the response will be.
                                 The temperature is clamped between 0.0 and 1.0
)";

    fmt::print("{}\n", messages);
}

std::string read_prompt_from_stdin_()
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Input: ");

    std::string prompt;
    std::getline(std::cin, prompt);

    return prompt;
}

MessagesOutput create_message_(const MessagesInput &input)
{
    threading::timer_enabled.store(true);
    std::thread timer(threading::time_api_call);

    bool query_failed = false;
    std::expected<MessagesOutput, Err> output;
    std::string errmsg;

    try {
        CreateMessage handle;
        output = handle.query_api(input);
    } catch (const std::runtime_error &e) {
        query_failed = true;
        errmsg = e.what();
    }

    threading::timer_enabled.store(false);
    timer.join();

    if (query_failed) {
        throw std::runtime_error(
            fmt::format("Failed to create message: '{}'", errmsg));
    }

    if (not output) {
        throw std::runtime_error(
            fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }

    return *output;
}

void print_output_to_stdout_(const MessagesOutput &output)
{
#ifdef TESTING_ENABLED
    const nlohmann::json json_obj = {
        { "input_tokens", output.input_tokens },
        { "llm_model", output.llm_model },
        { "output", output.output },
        { "output_tokens", output.output_tokens },
        { "stop_reason", output.stop_reason },
        { "temperature", output.temperature }
    };
    const std::string json_str = json_obj.dump(4);
    fmt::print("{}\n", json_str);
#else
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Output: ");
    fmt::print(fg(green), "{}\n", output.output);
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Usage:\n");
    fmt::print("Model: {}\n", output.llm_model);
    fmt::print("Temperature: {}\n", output.temperature);
    fmt::print("Input tokens: {}\n", output.input_tokens);
    fmt::print("Output tokens: {}\n", output.output_tokens);
    fmt::print("Stop reason: {}\n", output.stop_reason);
    fmt::print("Round trip time: {} s\n", output.rtt_time);
    utils::print_line();
#endif
}

} // namespace

namespace commands {

void command_run(const int argc, char **argv)
{
    MessagesInput input;
    std::string prompt;
    bool print_raw_response = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "limit", required_argument, 0, 'l' },
            { "raw", no_argument, 0, 'r' },
            { "system", required_argument, 0, 's' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:l:rs:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'm':
                input.set_llm_model(optarg);
                break;
            case 'p':
                prompt = optarg;
                break;
            case 'l':
                input.set_max_tokens(utils::string_to_int(optarg));
                break;
            case 'r':
                print_raw_response = true;
                break;
            case 's':
                input.set_system_prompt(optarg);
                break;
            case 't':
                input.set_temperature(utils::string_to_float(optarg));
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (prompt.empty()) {
        prompt = read_prompt_from_stdin_();
    }

    if (prompt.empty()) {
        throw std::runtime_error("The prompt is empty");
    } else {
        input.append_user_message(prompt);
    }

    const MessagesOutput output = create_message_(input);

    if (print_raw_response) {
        fmt::print("{}\n", output.raw_response);
    } else {
        print_output_to_stdout_(output);
    }
}

} // namespace commands
