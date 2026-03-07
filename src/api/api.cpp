#include "api.hpp"

#include "utils.hpp"

#include <cstdlib>
#include <json.hpp>
#include <stdexcept>

namespace {

size_t write_callback_(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string load_user_api_key_()
{
    const char *api_key = std::getenv("ANTHROPIC_API_KEY");

    if (api_key == nullptr) {
        throw std::runtime_error("Environment variable ANTHROPIC_API_KEY not found.");
    }

    return std::string(api_key);
}

std::string get_user_api_key_()
{
    static std::string api_key = load_user_api_key_();
    return api_key;
}

} // namespace

namespace api {

std::string unpack_error(const std::string &output)
{
    const nlohmann::json json = utils::parse_json(output);

    if (not json.contains("error")) {
        throw std::runtime_error("Malformed error response. No error object could be found");
    }

    if (not json["error"].contains("message")) {
        throw std::runtime_error("Malformed error response. No message could be found");
    }

    return json["error"]["message"];
}

Curl::Curl()
{
    this->user_api_key_ = get_user_api_key_();

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->curl_ = curl_easy_init();

    if (this->curl_ == nullptr) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    curl_easy_setopt(this->curl_, CURLOPT_WRITEFUNCTION, write_callback_);
}

Curl::~Curl()
{
    if (this->curl_) {
        curl_easy_cleanup(this->curl_);
    }

    curl_global_cleanup();
}

} // namespace api
