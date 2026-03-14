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

void print_help_messages_()
{
    const std::string messages = R"(Create a message according to a prompt.
Messaging using the run command is stateless.
The prompt can be read in interactively or via command line argument.

Usage:
  claudifier run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -p, --prompt=PROMPT            Specify the prompt message
  -l, --limit=LIMIT              Specify token limit
  -r, --raw                      Print raw response from server
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

ModelMessages read_cli_(const int argc, char **argv)
{
    ModelMessages model;
    std::string prompt;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "limit", required_argument, 0, 'l' },
            { "raw", no_argument, 0, 'r' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:l:r", long_options, &option_index);

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
            case 'p':
                prompt = optarg;
                break;
            case 'l':
                model.set_max_tokens(utils::string_to_int(optarg));
                break;
            case 'r':
                model.print_raw_response = true;
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
        model.append_user_message(prompt);
    }

    return model;
}

// ----------------------------------------------------------------------------------------------------------

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
    const ModelMessages model = read_cli_(argc, argv);
    const ModelMessagesResult model_result = create_message_(model);

    if (model.print_raw_response) {
        fmt::print("{}\n", model_result.raw_response);
    } else {
        print_results_to_stdout_(model_result);
    }
}

} // namespace commands
