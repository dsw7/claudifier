#pragma once

#include <curl/curl.h>
#include <string>

namespace api {

class CurlBase {
public:
    CurlBase();
    ~CurlBase();

    CurlBase(const CurlBase &) = delete;
    CurlBase &operator=(const CurlBase &) = delete;

protected:
    CURL *curl_ = nullptr;
    std::string user_api_key_;
};

} // namespace api
