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
  -d, --days=DAYS                Show usages DAYS numbers of days back. Value
                                 is clamped between 1 and 30 days
  -j, --json                     Export cost report as JSON
)";

    fmt::print("{}\n", messages);
}

void print_costs_(const CostReport &report)
{
    fmt::print("{:<25}{:<25}{}\n", "Starting at", "Ending at", "Cost (cents)");
    fmt::print("{:<25}{:<25}{}\n", "--------------------", "--------------------", "------------");

    float total_cost_dollars = 0.0f;
    int num_days = 0;

    for (const auto &bucket: report.get_cost_buckets()) {
        if (bucket.amount != "-") {
            fmt::print("{:<25}{:<25}{}\n", bucket.starting_at, bucket.ending_at, bucket.amount);
        } else {
            const float cost_cents = utils::string_to_float(bucket.amount);
            const float cost_dollars = cost_cents / 100.0f;
            fmt::print("{:<25}{:<25}{}\n", bucket.starting_at, bucket.ending_at, cost_dollars);
            total_cost_dollars += cost_dollars;
        }
        num_days++;
    }

    if (num_days > 1) {
        fmt::print("\nOverall usage (USD): ${} over {} days\n", total_cost_dollars, num_days);
    }
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

    days = std::clamp(days, 1, 30);

    GetCosts api_handle;
    const std::expected<CostReport, Err> output = api_handle.query_api(days);

    if (not output) {
        throw std::runtime_error(
            fmt::format("An error occurred when getting cost report: '{}'", output.error().errmsg));
    }

    if (dump_json) {
        fmt::print("{}\n", output->raw_response);
    } else {
        print_costs_(*output);
    }
}

} // namespace commands
