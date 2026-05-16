#include "command_run.hpp"

#include "query_messages_api.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

using api::CreateMessage;
using api::Err;
using api::MessagesOutput;

struct Parameters {
    bool print_json = false;
    bool print_raw_response = false;
    float temperature = 1.0f;
    int max_tokens = 1024;
    std::optional<std::string> system_prompt;
    std::optional<std::string> user_prompt;
    std::optional<std::string> model;
};

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
  -j, --json                     Dump results to JSON (as opposed to tabular format)
)";

    fmt::print("{}\n", messages);
}

std::string select_prompt_()
{
    std::string prompt;

    static std::filesystem::path inputfile = "Inputfile";
    if (std::filesystem::exists(inputfile)) {
#ifndef TESTING_ENABLED
        utils::print_line();
        fmt::print("Found Inputfile in current directory.\nLoading prompt from file.\n");
#endif
        prompt = utils::read_from_file(inputfile);
    } else {
        utils::print_line();
        prompt = utils::read_input_from_stdin();
    }

    return prompt;
}

MessagesOutput query_api_(CreateMessage &input)
{
    threading::timer_enabled.store(true);
    std::thread timer(threading::time_api_call);

    bool query_failed = false;
    std::expected<MessagesOutput, Err> output;
    std::string errmsg;

    try {
        output = input.query_api();
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
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Output: ");
    fmt::print(fg(colors::green), "{}\n", output.get_latest_text());
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Usage:\n");
    fmt::print("Model: {}\n", output.llm_model);
    fmt::print("Temperature: {}\n", output.temperature);
    fmt::print("Input tokens: {}\n", output.input_tokens);
    fmt::print("Output tokens: {}\n", output.output_tokens);
    fmt::print("Stop reason: {}\n", output.stop_reason);
    fmt::print("Round trip time: {} s\n", output.rtt_time);
    utils::print_line();
}

void print_output_to_stdout_json_(const MessagesOutput &output)
{
    const nlohmann::json json_obj = {
        { "input_tokens", output.input_tokens },
        { "llm_model", output.llm_model },
        { "output", output.get_latest_text() },
        { "output_tokens", output.output_tokens },
        { "stop_reason", output.stop_reason },
        { "temperature", output.temperature }
    };
    const std::string json_str = json_obj.dump(4);
    fmt::print("{}\n", json_str);
}

void create_message_(const Parameters &params)
{
    CreateMessage input(params.max_tokens, params.temperature, *params.model);

    std::string user_prompt;

    if (params.user_prompt) {
        user_prompt = *params.user_prompt;
    } else {
        user_prompt = select_prompt_();
    }

    if (user_prompt.empty()) {
        throw std::runtime_error("The prompt is empty");
    }

    input.append_user_message(user_prompt);

    if (params.system_prompt) {
        input.set_system_prompt(*params.system_prompt);
    }

    const MessagesOutput output = query_api_(input);

    if (params.print_raw_response) {
        fmt::print("{}\n", output.raw_response);
    } else if (params.print_json) {
        print_output_to_stdout_json_(output);
    } else {
        print_output_to_stdout_(output);
    }
}

} // namespace

namespace commands {

void command_run(const int argc, char **argv, const ConfigsRun &configs)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "limit", required_argument, 0, 'l' },
            { "raw", no_argument, 0, 'r' },
            { "system", required_argument, 0, 's' },
            { "temperature", required_argument, 0, 't' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:l:rs:t:j", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'm':
                params.model = optarg;
                break;
            case 'p':
                params.user_prompt = optarg;
                break;
            case 'l':
                params.max_tokens = utils::string_to_int(optarg);
                break;
            case 'r':
                params.print_raw_response = true;
                break;
            case 's':
                params.system_prompt = optarg;
                break;
            case 't':
                params.temperature = utils::string_to_float(optarg);
                break;
            case 'j':
                params.print_json = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (not params.model) {
        params.model = configs.model;
    }

    create_message_(params);
}

} // namespace commands
