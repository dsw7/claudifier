#include "command_models.hpp"

#include "command_utils.hpp"
#include "query_models_api.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <stdexcept>

namespace {

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

ListModels read_cli_(const int argc, char **argv)
{
    ListModels model;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "limit", required_argument, 0, 'l' },
            { "json", required_argument, 0, 'j' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hl:j:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'l':
                model.set_max_items_per_page(utils::string_to_int(optarg));
                break;
            case 'j':
                model.dump_json = true;
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} models [-h | --help] for more information", argv[0]));
        }
    };

    return model;
}

// ----------------------------------------------------------------------------------------------------------

void print_page_(const ListModelsPage &page)
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
        const std::expected<ListModelsPage, Err> model_result = handle.query_api(limit, last_id);

        if (not model_result) {
            throw std::runtime_error(
                fmt::format("An error occurred when getting list of models: '{}'", model_result.error().errmsg));
        }

        print_page_(*model_result);
        last_id = model_result->last_id;

        if (not model_result->has_more) {
            break;
        }
    }
}

} // namespace

namespace commands {

void command_models(const int argc, char **argv)
{
    const ListModels model = read_cli_(argc, argv);
    print_all_pages_(model.get_max_items_per_page());
}

} // namespace commands
