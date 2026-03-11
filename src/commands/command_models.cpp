#include "command_models.hpp"

#include "query_models_api.hpp"
#include "read_cli.hpp"

#include <fmt/core.h>
#include <optional>
#include <stdexcept>

namespace {

void get_models_list_(int limit)
{
    api::GetModels handle;
    std::optional<std::string> last_id;

    while (true) {
        const std::expected<ModelListModelsResult, Err> model_result = handle.query_api(limit, last_id);

        if (not model_result) {
            throw std::runtime_error(model_result.error().errmsg);
        }

        for (const auto &data: model_result->data) {
            fmt::print("{} {}\n", data.id, data.created_at);
        }

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
