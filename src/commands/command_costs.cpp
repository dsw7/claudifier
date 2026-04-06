#include "command_costs.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>

namespace {

void print_help_messages_()
{
    const std::string messages = R"(Get cost report. Note that this command requires a valid Anthropic
administrative API key.

Usage:
  claudifier costs [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -j, --json                     Export cost report as JSON
)";

    fmt::print("{}\n", messages);
}

} // namespace

namespace commands {

void command_costs(const int argc, char **argv)
{
    bool dump_json = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hj", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'j':
                dump_json = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} costs [-h | --help] for more information", argv[0]));
        }
    };

    if (dump_json) {
    } else {
    }
}

} // namespace commands
