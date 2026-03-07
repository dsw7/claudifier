#include "api.hpp"

#include "parse_utils.hpp"

#include <json.hpp>
#include <stdexcept>

namespace {

std::string pack_post_fields_(const std::string &content)
{
    const nlohmann::json post_fields = {
        { "max_tokens", 1024 },
        { "messages", { { { "content", content }, { "role", "user" } } } },
        { "model", "claude-opus-4-6" }
    };

    return post_fields.dump();
}

} // namespace

namespace api {

MessageResult Curl::create_message(const std::string &input)
{
    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + this->user_api_key_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, headers);

    const std::string post_fields = pack_post_fields_(input);
    curl_easy_setopt(this->curl_, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(this->curl_);

    curl_slist_free_all(headers);
    headers = nullptr;

    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }

    long http_status_code = -1;
    curl_easy_getinfo(this->curl_, CURLINFO_RESPONSE_CODE, &http_status_code);

    if (http_status_code == 200) {
        return OkMessage { response };
    }

    return std::unexpected(Err { http_status_code, unpack_error(response) });
}

} // namespace api
