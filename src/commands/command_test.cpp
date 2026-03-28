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
  -o, --one-shot                 Run one-shot prompting test 
)";

    fmt::print("{}\n", messages);
}

struct TestCase {
    bool zero_shot = false;
    bool one_shot = false;
};

TestCase read_cli_(const int argc, char **argv)
{
    TestCase test_case;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "zero-shot", no_argument, 0, 'z' },
            { "one-shot", no_argument, 0, 'o' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hzo", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'z':
                test_case.zero_shot = true;
                break;
            case 'o':
                test_case.one_shot = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    return test_case;
}

void test_zero_shot_()
{
    Messages model;
    model.append_user_message("What is 3 + 5?");

    api::CreateMessage handle;
    std::expected<MessagesResult, Err> result = handle.query_api(model);

    if (result) {
        fmt::print("{}\n", result->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", result.error().errmsg));
    }
}

void test_one_shot_()
{
    Messages model;
    model.append_user_message("This result is great!");
    model.append_assistant_message("This message sounds POSITIVE");
    model.append_user_message("This result is bad!");

    api::CreateMessage handle;
    std::expected<MessagesResult, Err> result = handle.query_api(model);

    if (result) {
        fmt::print("{}\n", result->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", result.error().errmsg));
    }
}

} // namespace

namespace commands {

void command_test(const int argc, char **argv)
{
    const TestCase test_case = read_cli_(argc, argv);

    if (test_case.zero_shot) {
        test_zero_shot_();
    }

    if (test_case.one_shot) {
        test_one_shot_();
    }
}

} // namespace commands
