#include "curl_base.hpp"

#include <json.hpp>
#include <stdexcept>
#include <string>

namespace {

size_t write_callback_(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

} // namespace

namespace api {

CurlBase::CurlBase()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->curl_ = curl_easy_init();

    if (this->curl_ == nullptr) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    curl_easy_setopt(this->curl_, CURLOPT_WRITEFUNCTION, write_callback_);
}

CurlBase::~CurlBase()
{
    if (this->curl_) {
        curl_easy_cleanup(this->curl_);
    }

    curl_global_cleanup();
}

bool CurlBase::is_200_response_()
{
    long http_status_code = -1;

    const CURLcode return_code = curl_easy_getinfo(this->curl_, CURLINFO_RESPONSE_CODE, &http_status_code);
    if (return_code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(return_code));
    }

    return http_status_code == 200;
}

double CurlBase::get_rtt_time_()
{
    double rtt_time = 0.0;

    const CURLcode return_code = curl_easy_getinfo(this->curl_, CURLINFO_TOTAL_TIME, &rtt_time);
    if (return_code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(return_code));
    }

    return rtt_time;
}

} // namespace api
