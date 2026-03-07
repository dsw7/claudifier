#pragma once

#include <curl/curl.h>
#include <expected>
#include <string>

namespace api {

struct Err {
    long code = -1;
    std::string message;
};

using MessageResult = std::expected<std::string, Err>;

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

std::string unpack_error(const std::string &output);

} // namespace api
