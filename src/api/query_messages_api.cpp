#include "query_messages_api.hpp"

#include "parse_utils.hpp"

#include <json.hpp>
#include <stdexcept>

namespace {

std::string model_to_post_fields_(const ModelMessages &model)
{
    const nlohmann::json post_fields = {
        { "max_tokens", model.token_limit },
        { "messages", { { { "content", model.prompt }, { "role", "user" } } } },
        { "model", model.llm_model }
    };

    return post_fields.dump();
}

ModelMessagesResult unpack_200_response_to_model_(const std::string &response)
{
    const nlohmann::json json = api::parse_response(response);

    if (json["type"] != "message") {
        throw std::runtime_error("Malformed message response. Response is not a message");
    }

    ModelMessagesResult ok;
    ok.input_tokens = json["usage"]["input_tokens"];
    ok.llm_model = json["model"];
    ok.output = json["content"][0]["text"];
    ok.output_tokens = json["usage"]["output_tokens"];
    ok.raw_response = json.dump(4);
    return ok;
}

} // namespace

namespace api {

std::expected<ModelMessagesResult, Err> CreateMessage::query_api(const ModelMessages &model)
{
    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + this->user_api_key_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, headers);

    const std::string post_fields = model_to_post_fields_(model);
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
        return unpack_200_response_to_model_(response);
    }

    return std::unexpected(unpack_error(response));
}

} // namespace api
