#include "command_test.hpp"

#include "query_messages_api.hpp"

#include <array>
#include <expected>
#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>

namespace {

using api::CreateMessage;
using api::Err;
using api::MessagesInput;
using api::MessagesOutput;

struct TestCase {
    bool chat = false;
    bool zero_shot = false;
    bool one_shot = false;
    bool few_shot = false;
    bool chain_of_thought = false;
    bool tree_of_thought = false;
};

void print_help_messages_()
{
    const std::string messages = R"(Internal test command.

Usage:
  claudifier test [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -u, --chat                     Run chat test
  -z, --zero-shot                Run zero-shot prompting test
  -o, --one-shot                 Run one-shot prompting test
  -f, --few-shot                 Run few-shot prompting test
  -c, --chain-of-thought         Run chain-of-thought test
  -t, --tree-of-thought          Run tree-of-thought test
)";

    fmt::print("{}\n", messages);
}

void test_chat_()
{
    // mock conversational turns like in chat command but without interactivity for testing
    CreateMessage handle;
    MessagesInput input;

    input.append_user_message("If a = 2, b = 3, and c = a + b, then what is c?");
    std::expected<MessagesOutput, Err> output_1 = handle.query_api(input);
    if (not output_1) {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output_1.error().errmsg));
    }
    input.append_assistant_message(output_1->output);

    input.append_user_message("What is c + 5? Return just the value");
    std::expected<MessagesOutput, Err> output_2 = handle.query_api(input);
    if (not output_2) {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output_2.error().errmsg));
    }
    input.append_assistant_message(output_2->output);

    fmt::print("{}\n", output_2->output);
}

void test_zero_shot_()
{
    MessagesInput input;
    input.append_user_message("What is 3 + 5?");

    CreateMessage handle;
    std::expected<MessagesOutput, Err> output = handle.query_api(input);

    if (output) {
        fmt::print("{}\n", output->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }
}

void test_one_shot_()
{
    MessagesInput input;
    input.append_user_message("This output is great!");
    input.append_assistant_message("This message sounds POSITIVE");
    input.append_user_message("This output is bad!");

    CreateMessage handle;
    std::expected<MessagesOutput, Err> output = handle.query_api(input);

    if (output) {
        fmt::print("{}\n", output->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }
}

void test_few_shot_()
{
    MessagesInput input;
    input.append_user_message("The value is a;");
    input.append_assistant_message("a = 52");
    input.append_user_message("The value is b;");
    input.append_assistant_message("b = 54");
    input.append_user_message("The value is c;");
    input.append_assistant_message("c = 56");
    input.append_user_message("The value is d;");

    CreateMessage handle;
    std::expected<MessagesOutput, Err> output = handle.query_api(input);

    if (output) {
        fmt::print("{}\n", output->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }
}

void test_chain_of_thought_()
{
    MessagesInput input;
    input.append_user_message(
        "x = 16. x is then doubled. Finally, x is divided by 4. What is the value of x? Show each step in your calculation.");

    CreateMessage handle;
    std::expected<MessagesOutput, Err> output = handle.query_api(input);

    if (output) {
        fmt::print("{}\n", output->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }
}

void test_tree_of_thought_()
{
    MessagesInput input;
    input.append_user_message(R"(Imagine three different experts are answering this question."
"All experts will write down 1 step of their thinking, then share it with the group."
"If any expert realizes they're wrong at any point then they leave."
"Goal: Use the numbers 4, 9, 10, 13 and basic operators (+, -, *, /) to get the number 24."
"Step 1:)");

    CreateMessage handle;
    std::expected<MessagesOutput, Err> output = handle.query_api(input);

    if (output) {
        fmt::print("{}\n", output->raw_response);
    } else {
        throw std::runtime_error(fmt::format("An error occurred when creating message: '{}'", output.error().errmsg));
    }
}

void run_test_cases_(const TestCase &test_case)
{
    if (test_case.chat) {
        test_chat_();
    }

    if (test_case.zero_shot) {
        test_zero_shot_();
    }

    if (test_case.one_shot) {
        test_one_shot_();
    }

    if (test_case.few_shot) {
        test_few_shot_();
    }

    if (test_case.chain_of_thought) {
        test_chain_of_thought_();
    }

    if (test_case.tree_of_thought) {
        test_tree_of_thought_();
    }
}

} // namespace

namespace commands {

void command_test(const int argc, char **argv)
{
    TestCase test_case;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "chat", no_argument, 0, 'u' },
            { "zero-shot", no_argument, 0, 'z' },
            { "one-shot", no_argument, 0, 'o' },
            { "few-shot", no_argument, 0, 'f' },
            { "chain-of-thought", no_argument, 0, 'c' },
            { "tree-of-thought", no_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "huzofct", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'u':
                test_case.chat = true;
                break;
            case 'z':
                test_case.zero_shot = true;
                break;
            case 'o':
                test_case.one_shot = true;
                break;
            case 'f':
                test_case.few_shot = true;
                break;
            case 'c':
                test_case.chain_of_thought = true;
                break;
            case 't':
                test_case.tree_of_thought = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} test [-h | --help] for more information", argv[0]));
        }
    };

    run_test_cases_(test_case);
}

} // namespace commands
