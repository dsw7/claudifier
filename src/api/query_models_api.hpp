#pragma once

#include "curl_base.hpp"
#include "models.hpp"

#include <optional>
#include <string>

namespace api {

class GetModels: public CurlBase {
public:
    std::expected<ListModelsPage, Err> query_api(
        const int limit,
        const std::optional<std::string> &last_id = std::nullopt);
};

} // namespace api
