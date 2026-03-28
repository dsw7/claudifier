#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace api {

struct ModelData {
    std::string created_at;
    std::string display_name;
    std::string id;
};

struct ModelsOutput {
    bool has_more = false;
    std::string last_id;
    std::string raw_response;
    std::vector<ModelData> data;

    void append_llm_model_to_page(const std::string &created_at, const std::string &display_name, const std::string &id);
};

class GetModels: public CurlBase {
public:
    std::expected<ModelsOutput, Err> query_api(
        const int limit,
        const std::optional<std::string> &last_id = std::nullopt);
};

} // namespace api
