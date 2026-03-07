#include "api.hpp"

#include <json.hpp>
#include <stdexcept>

namespace {

using json = nlohmann::json;

} // namespace

namespace api {

std::string Curl::create_message(const std::string &input)
{
    this->headers_ = curl_slist_append(this->headers_, ("X-Api-Key: " + this->user_api_key_).c_str());
    this->headers_ = curl_slist_append(this->headers_, "Content-Type: application/json");
    this->headers_ = curl_slist_append(this->headers_, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, this->headers_);

    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);
    const json post_fields_json = {
        { "max_tokens", 1024 },
        { "messages", { { { "content", input }, { "role", "user" } } } },
        { "model", "claude-opus-4-6" }
    };
    const std::string post_fields_str = post_fields_json.dump();
    curl_easy_setopt(this->curl_, CURLOPT_POSTFIELDS, post_fields_str.c_str());

    std::string output;
    curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &output);

    const CURLcode code = curl_easy_perform(this->curl_);
    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }

    long http_status_code = -1;
    curl_easy_getinfo(this->curl_, CURLINFO_RESPONSE_CODE, &http_status_code);

    if (http_status_code == 200) {
        return output;
    }

    throw std::runtime_error(output);
}

} // namespace api
