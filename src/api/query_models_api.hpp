#pragma once

#include "curl_base.hpp"
#include "models.hpp"

namespace api {

class GetModels: public CurlBase {
public:
    std::expected<ModelListModelsResult, Err> query_api();
};

} // namespace api
