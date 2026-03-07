#include "command_run.hpp"

#include "api.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>

namespace {

void help_run_command_()
{
    const std::string messages = R"(Create a message according to a prompt. Messaging
using the run command is stateless. The prompt is read in interactively.

Usage:
  cl run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
)";

    fmt::print("{}\n", messages);
}

void create_message_(const std::string &model)
{
    api::Curl curl;
    const auto result = curl.create_message("What is 3 + 5?", model);

    if (result) {
        fmt::print("{}\n", result->output);
    } else {
        throw std::runtime_error(result.error().errmsg);
    }
}

} // namespace

namespace commands {

void command_run(int argc, char **argv)
{
    bool print_help = false;
    std::string model = "claude-opus-4-6";

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:", long_options, &option_index);

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
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (print_help) {
        help_run_command_();
        return;
    }

    create_message_(model);
}

} // namespace commands
