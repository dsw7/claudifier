#include "command_models.hpp"

#include "query_models_api.hpp"
#include "read_cli.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

ModelListModelsResult get_models_list_(int limit)
{
    api::GetModels handle;
    const std::expected<ModelListModelsResult, Err> model_result = handle.query_api(limit);

    if (not model_result) {
        throw std::runtime_error(model_result.error().errmsg);
    }

    return *model_result;
}

} // namespace

namespace commands {

void command_models(int argc, char **argv)
{
    ModelListModels model;
    read_cli(argc, argv, model);

    const ModelListModelsResult model_result = get_models_list_(model.limit);
    fmt::print("{}\n", model_result.raw_response);
}

} // namespace commands
