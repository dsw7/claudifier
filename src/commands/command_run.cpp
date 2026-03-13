#include "command_run.hpp"

#include "command_utils.hpp"
#include "query_messages_api.hpp"
#include "read_cli.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
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

void preprocess_and_validate_params_(const ModelMessages &model)
{
    if (model.prompt.empty()) {
        throw std::runtime_error("The prompt is empty");
    }

    if (model.llm_model.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }
}

std::string read_prompt_from_stdin_()
{
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Input: ");

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
        throw std::runtime_error(
            fmt::format("Failed to create message: '{}'", errmsg));
    }

    if (not model_result) {
        throw std::runtime_error(
            fmt::format("An error occurred when creating message: '{}'", model_result.error().errmsg));
    }

    return *model_result;
}

void print_results_to_stdout_(const ModelMessagesResult &model)
{
#ifdef TESTING_ENABLED
    const nlohmann::json json_obj = {
        { "output", model.output },
        { "llm_model", model.llm_model },
        { "input_tokens", model.input_tokens },
        { "output_tokens", model.output_tokens }
    };
    const std::string json_str = json_obj.dump(4);
    fmt::print("{}\n", json_str);
#else
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Output: ");
    fmt::print(fg(green), "{}\n", model.output);
    utils::print_line();
    fmt::print(fmt::emphasis::bold, "Usage:\n");
    fmt::print("Model: {}\n", model.llm_model);
    fmt::print("Input tokens: {}\n", model.input_tokens);
    fmt::print("Output tokens: {}\n", model.output_tokens);
    utils::print_line();
#endif
}

} // namespace

namespace commands {

void command_run(const int argc, char **argv)
{
    ModelMessages model;
    read_cli(argc, argv, model);

    if (model.prompt.empty()) {
        model.prompt = read_prompt_from_stdin_();
    }

    preprocess_and_validate_params_(model);
    const ModelMessagesResult model_result = create_message_(model);

    if (model.print_raw_response) {
        fmt::print("{}\n", model_result.raw_response);
    } else {
        print_results_to_stdout_(model_result);
    }
}

} // namespace commands
