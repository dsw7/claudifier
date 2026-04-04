#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
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
    std::string raw_response;
    std::vector<ModelData> data;
};

class GetModels: public CurlBase {
public:
    std::expected<ModelsOutput, Err> query_api();
};

} // namespace api
