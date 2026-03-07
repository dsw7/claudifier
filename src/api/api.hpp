#pragma once

#include "responses.hpp"

#include <curl/curl.h>
#include <string>

namespace api {

class Curl {
public:
    Curl();
    ~Curl();

    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

    MessageResult create_message(const std::string &input, const std::string &model);

private:
    CURL *curl_ = nullptr;
    std::string user_api_key_;
};

} // namespace api
