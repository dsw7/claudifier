#include "command_run.hpp"

#include "command_utils.hpp"
#include "query_messages_api.hpp"
#include "read_cli.hpp"

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

ModelMessagesResult create_message_(const ModelMessages &model)
{
    threading::timer_enabled.store(true);
    std::thread timer(threading::time_api_call);

    bool query_failed = false;
    std::expected<ModelMessagesResult, Err> model_result;
    std::string errmsg;

    try {
        api::CreateMessage handle;
        model_result = handle.query_api(model);
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

    if (not model_result) {
        throw std::runtime_error(model_result.error().errmsg);
    }

    return *model_result;
}

void print_results_(const ModelMessagesResult &model)
{
    utils::print_line();
    fmt::print("\033[1mOutput: \033[32m{}\033[0m\n", model.output);
    utils::print_line();
    fmt::print("\033[1mUsage:\033[0m\n");
    fmt::print("Model: {}\n", model.llm_model);
    fmt::print("Input tokens: {}\n", model.input_tokens);
    fmt::print("Output tokens: {}\n", model.output_tokens);
    utils::print_line();
}

} // namespace

namespace commands {

void command_run(int argc, char **argv)
{
    ModelMessages model;
    read_cli(argc, argv, model);

    if (model.prompt.empty()) {
        model.prompt = read_prompt_from_stdin_();
    }

    const ModelMessagesResult model_result = create_message_(model);
    print_results_(model_result);
}

} // namespace commands
