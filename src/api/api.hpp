#pragma once

#include "models.hpp"

#include <curl/curl.h>
#include <string>

namespace api {

class Curl {
public:
    Curl();
    ~Curl();

    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

    std::expected<ModelMessagesResult, Err> create_message(const ModelMessages &model);

private:
    CURL *curl_ = nullptr;
    std::string user_api_key_;
};

} // namespace api
