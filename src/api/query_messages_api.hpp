#pragma once

#include "curl_base.hpp"
#include "models.hpp"

namespace api {

class CreateMessage: public CurlBase {
public:
    std::expected<MessagesResult, Err> query_api(const Messages &model);
};

} // namespace api
