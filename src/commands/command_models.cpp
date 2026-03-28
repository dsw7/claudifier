#include "command_models.hpp"

#include "command_utils.hpp"
#include "query_models_api.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <json.hpp>
#include <optional>
#include <stdexcept>

namespace {

using api::ModelsOutput;

void print_help_messages_()
{
    const std::string messages = R"(Get a list of available models.

Usage:
  claudifier models [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -l, --limit=LIMIT              Specify LIMIT number of models per page. LIMIT
                                 is clamped between 1 and 1000.
  -j, --json                     Export list of models as JSON
)";

    fmt::print("{}\n", messages);
}

void print_page_(const ModelsOutput &page)
{
    static bool print_header = true;

    if (print_header) {
        fmt::print("{:<10}{:<25}{:<25}{}\n", "Page", "Display name", "Created at", "Model ID");
        fmt::print("{:<10}{:<25}{:<25}{}\n", "----", "------------", "----------", "--------");
        print_header = false;
    }

    static int page_num = 1;

    for (const auto &data: page.data) {
        fmt::print("{:<10}{:<25}{:<25}{}\n", page_num, data.display_name, data.created_at, data.id);
    }

    page_num++;
}

void print_all_pages_(const int limit)
{
    api::GetModels handle;
    std::optional<std::string> last_id;

    while (true) {
        const std::expected<ModelsOutput, Err> output = handle.query_api(limit, last_id);

        if (not output) {
            throw std::runtime_error(
                fmt::format("An error occurred when getting list of models: '{}'", output.error().errmsg));
        }

        print_page_(*output);
        last_id = output->last_id;

        if (not output->has_more) {
            break;
        }
    }
}

void print_all_pages_as_json_(const int limit)
{
    api::GetModels handle;
    std::optional<std::string> last_id;
    nlohmann::json json = nlohmann::json::array();
    int page_num = 1;

    while (true) {
        const std::expected<ModelsOutput, Err> output = handle.query_api(limit, last_id);

        if (not output) {
            throw std::runtime_error(
                fmt::format("An error occurred when getting list of models: '{}'", output.error().errmsg));
        }

        for (const auto &data: output->data) {
            json.push_back({ { "page", page_num },
                { "display_name", data.display_name },
                { "created_at", data.created_at },
                { "id", data.id } });
        }
        page_num++;
        last_id = output->last_id;

        if (not output->has_more) {
            break;
        }
    }

    fmt::print("{}\n", json.dump(4));
}

} // namespace

namespace commands {

void command_models(const int argc, char **argv)
{
    bool dump_json = false;
    int items_per_page = 100;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "limit", required_argument, 0, 'l' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hl:j", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'l':
                items_per_page = utils::string_to_int(optarg);
                break;
            case 'j':
                dump_json = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} models [-h | --help] for more information", argv[0]));
        }
    };

    items_per_page = std::clamp(items_per_page, 1, 1000);

    if (dump_json) {
        print_all_pages_as_json_(items_per_page);
    } else {
        print_all_pages_(items_per_page);
    }
}

} // namespace commands
