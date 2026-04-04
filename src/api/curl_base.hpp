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
    bool is_200_response_();
    double get_rtt_time_();

    CURL *curl_ = nullptr;
    std::string user_api_key_;
};

} // namespace api
