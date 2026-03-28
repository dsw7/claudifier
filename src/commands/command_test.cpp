#include "command_test.hpp"

#include "query_messages_api.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>

namespace {

void print_help_messages_()
{
    const std::string messages = R"(Internal test command.

Usage:
  claudifier test [OPTIONS]

Options:
  -h, --help                     Print help information and exit
)";

    fmt::print("{}\n", messages);
}

void read_cli_(const int argc, char **argv)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "h", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };
}

void test_zero_shot_()
{
    Messages model;
    model.append_user_message("What is 3 + 5?");

    api::CreateMessage api_handle;
    std::expected<MessagesResult, Err> result = api_handle.query_api(model);

    if (not result) {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", result.error().errmsg));
    }

    fmt::print("{}\n", result->output);
}

} // namespace

namespace commands {

void command_test(const int argc, char **argv)
{
    read_cli_(argc, argv);
    test_zero_shot_();
}

} // namespace commands
