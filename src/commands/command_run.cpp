#include "command_run.hpp"

#include "api.hpp"
#include "command_utils.hpp"
#include "read_cli.hpp"
#include "responses.hpp"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

std::string read_prompt_from_stdin_()
{
    utils::print_line();
    fmt::print("\033[1mInput:\033[0m ");

    std::string prompt;
    std::getline(std::cin, prompt);

    return prompt;
}

MessageResult create_message_(const ModelMessages &mod_messages)
{
    threading::timer_enabled.store(true);
    std::thread timer(threading::time_api_call);

    bool query_failed = false;
    MessageResult result;
    std::string errmsg;

    try {
        api::Curl curl;
        result = curl.create_message(mod_messages);
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
    utils::print_line();
    fmt::print("\033[1mOutput: \033[32m{}\033[0m\n", result->output);
    utils::print_line();
    fmt::print("\033[1mUsage:\033[0m\n");
    fmt::print("Model: {}\n", result->model);
    fmt::print("Input tokens: {}\n", result->input_tokens);
    fmt::print("Output tokens: {}\n", result->output_tokens);
    utils::print_line();
}

} // namespace

namespace commands {

void command_run(int argc, char **argv)
{
    ModelMessages mod_messages;
    read_cli(argc, argv, mod_messages);

    if (mod_messages.prompt.empty()) {
        mod_messages.prompt = read_prompt_from_stdin_();
    }

    const MessageResult result = create_message_(mod_messages);
    print_results_(result);
}

} // namespace commands
