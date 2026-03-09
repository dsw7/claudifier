#pragma once

#include "curl_base.hpp"
#include "models.hpp"

namespace api {

class CreateMessage: public CurlBase {
public:
    std::expected<ModelMessagesResult, Err> query_api(const ModelMessages &model);
};

} // namespace api
