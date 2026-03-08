#include "command_run.hpp"

#include "api.hpp"
#include "command_utils.hpp"
#include "responses.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

void help_run_command_()
{
    const std::string messages = R"(Create a message according to a prompt.
Messaging using the run command is stateless.
The prompt can be read in interactively or via command line argument.

Usage:
  cl run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -p, --prompt=PROMPT            Specify the prompt message
)";

    fmt::print("{}\n", messages);
}

std::string select_prompt_(const std::optional<std::string> &prompt_from_cli)
{
    if (prompt_from_cli.has_value()) {
        return *prompt_from_cli;
    }

    std::string prompt_from_stdin;
    print_line();
    fmt::print("\033[1mInput:\033[0m ");
    std::getline(std::cin, prompt_from_stdin);
    return prompt_from_stdin;
}

MessageResult create_message_(const std::string &prompt, const std::string &model)
{
    threading::timer_enabled.store(true);
    std::thread timer(threading::time_api_call);

    bool query_failed = false;
    MessageResult result;
    std::string errmsg;

    try {
        api::Curl curl;
        result = curl.create_message(prompt, model);
    } catch (const std::runtime_error &e) {
        query_failed = true;
        errmsg = e.what();
    }

    threading::timer_enabled.store(false);
    timer.join();

    if (query_failed) {
        fmt::print(stderr, "{}\n", errmsg);
        throw std::runtime_error("Cannot proceed");
    }

    if (not result) {
        throw std::runtime_error(result.error().errmsg);
    }

    return result;
}

void print_results_(const MessageResult &result)
{
    print_line();
    fmt::print("\033[1mOutput: \033[32m{}\033[0m\n", result->output);
    print_line();
    fmt::print("\033[1mUsage:\033[0m\n");
    fmt::print("Model: {}\n", result->model);
    fmt::print("Input tokens: {}\n", result->input_tokens);
    fmt::print("Output tokens: {}\n", result->output_tokens);
    print_line();
}

} // namespace

namespace commands {

void command_run(int argc, char **argv)
{
    bool print_help = false;
    std::string model = "claude-opus-4-6";
    std::optional<std::string> prompt_from_cli;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help = true;
                break;
            case 'm':
                model = optarg;
                break;
            case 'p':
                prompt_from_cli = optarg;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (print_help) {
        help_run_command_();
        return;
    }

    const std::string prompt = select_prompt_(prompt_from_cli);
    const MessageResult result = create_message_(prompt, model);
    print_results_(result);
}

} // namespace commands
