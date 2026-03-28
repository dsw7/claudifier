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
  -z, --zero-shot                Run zero-shot prompting test 
)";

    fmt::print("{}\n", messages);
}

enum class TestCase {
    ZERO_SHOT,
    ONE_SHOT,
    FEW_SHOT,
};

TestCase read_cli_(const int argc, char **argv)
{
    TestCase tc;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "zero-shot", no_argument, 0, 'z' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hz", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'z':
                tc = TestCase::ZERO_SHOT;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    return tc;
}

void test_zero_shot_()
{
    Messages model;
    model.append_user_message("What is 3 + 5?");

    api::CreateMessage handle;
    std::expected<MessagesResult, Err> result = handle.query_api(model);

    if (result) {
        fmt::print("{}\n", result->output);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", result.error().errmsg));
    }
}

} // namespace

namespace commands {

void command_test(const int argc, char **argv)
{
    TestCase tc = read_cli_(argc, argv);

    switch (tc) {
        case TestCase::ZERO_SHOT:
            test_zero_shot_();
            break;
        default:
            test_zero_shot_();
    }
}

} // namespace commands
