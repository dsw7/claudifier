#pragma once

#include <curl/curl.h>

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
};

} // namespace api
