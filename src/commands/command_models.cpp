#include "command_models.hpp"

#include "query_models_api.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <json.hpp>
#include <optional>
#include <stdexcept>

namespace {

using api::Err;
using api::GetModels;
using api::ModelsOutput;

void print_help_messages_()
{
    const std::string messages = R"(Get a list of available models.

Usage:
  claudifier models [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -j, --json                     Export list of models as JSON
)";

    fmt::print("{}\n", messages);
}

ModelsOutput get_models_()
{
    GetModels api_handle;
    const std::expected<ModelsOutput, Err> output = api_handle.query_api();

    if (not output) {
        throw std::runtime_error(
            fmt::format("An error occurred when getting list of models: '{}'", output.error().errmsg));
    }

    return *output;
}

void print_all_pages_()
{
    const ModelsOutput models = get_models_();

    fmt::print("{:<25}{:<25}{}\n", "Display name", "Created at", "Model ID");
    fmt::print("{:<25}{:<25}{}\n", "------------", "----------", "--------");

    for (const auto &data: models.data) {
        fmt::print("{:<25}{:<25}{}\n", data.display_name, data.created_at, data.id);
    }
}

void print_all_pages_as_json_()
{
    const ModelsOutput models = get_models_();

    nlohmann::json json = nlohmann::json::array();
    for (const auto &data: models.data) {
        json.push_back({ { "display_name", data.display_name },
            { "created_at", data.created_at },
            { "id", data.id } });
    }

    fmt::print("{}\n", json.dump(4));
}

} // namespace

namespace commands {

void command_models(const int argc, char **argv)
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
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} models [-h | --help] for more information", argv[0]));
        }
    };

    if (dump_json) {
        print_all_pages_as_json_();
    } else {
        print_all_pages_();
    }
}

} // namespace commands
