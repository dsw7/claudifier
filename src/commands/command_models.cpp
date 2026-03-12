#include "command_models.hpp"

#include "query_models_api.hpp"
#include "read_cli.hpp"

#include <fmt/core.h>
#include <optional>
#include <stdexcept>

namespace {

void print_page_(const ModelListModelsResult &page)
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

void get_models_list_(const int limit)
{
    api::GetModels handle;
    std::optional<std::string> last_id;

    while (true) {
        const std::expected<ModelListModelsResult, Err> model_result = handle.query_api(limit, last_id);

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

void command_models(int argc, char **argv)
{
    ModelListModels model;
    read_cli(argc, argv, model);

    get_models_list_(model.limit);
}

} // namespace commands
