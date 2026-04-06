#include "command_costs.hpp"

#include "query_costs_api.hpp"
#include "utils.hpp"

#include <algorithm>
#include <expected>
#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>

namespace {

using api::CostReport;
using api::Err;
using api::GetCosts;

void print_help_messages_()
{
    const std::string messages = R"(Get cost report. Note that this command requires a valid Anthropic
administrative API key.

Usage:
  claudifier costs [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -d, --days=DAYS                Show usages DAYS numbers of days back
  -j, --json                     Export cost report as JSON
)";

    fmt::print("{}\n", messages);
}

} // namespace

namespace commands {

void command_costs(const int argc, char **argv)
{
    bool dump_json = false;
    int days = 30;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "days", required_argument, 0, 'd' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hjd:", long_options, &option_index);

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
            case 'd':
                days = utils::string_to_int(optarg);
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} costs [-h | --help] for more information", argv[0]));
        }
    };

    days = std::clamp(days, 1, 60);

    GetCosts api_handle;
    const std::expected<CostReport, Err> output = api_handle.query_api(days);

    if (not output) {
        throw std::runtime_error(
            fmt::format("An error occurred when getting cost report: '{}'", output.error().errmsg));
    }

    if (dump_json) {
        fmt::print("{}\n", output->raw_response);
    }
}

} // namespace commands
