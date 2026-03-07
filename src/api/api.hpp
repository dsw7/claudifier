#pragma once

#include <curl/curl.h>
#include <expected>
#include <string>

namespace api {

struct OkMessage {
    std::string message;
};

struct Err {
    long code = -1;
    std::string message;
};

using MessageResult = std::expected<OkMessage, Err>;

class Curl {
public:
    Curl();
    ~Curl();

    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

    MessageResult create_message(const std::string &input);

private:
    CURL *curl_ = nullptr;
    std::string user_api_key_;
};

} // namespace api
