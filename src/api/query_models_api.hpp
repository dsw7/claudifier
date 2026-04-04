#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

namespace api {

struct ModelData {
    std::string created_at;
    std::string display_name;
    std::string id;
};

class ModelsOutput {
public:
    ModelsOutput(const std::string &response);

    bool has_more = false;
    std::string raw_response;
    std::vector<ModelData> data;

private:
    nlohmann::json response_;
    void validate_schema_();
};

class GetModels: public CurlBase {
public:
    std::expected<ModelsOutput, Err> query_api();
};

} // namespace api
