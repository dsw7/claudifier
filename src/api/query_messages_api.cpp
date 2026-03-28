#include "query_messages_api.hpp"

#include "parse_utils.hpp"

#include <json.hpp>
#include <stdexcept>

namespace {

MessagesResult unpack_200_response_(const std::string &response)
{
    const nlohmann::json json = api::parse_response(response);

    if (not json.contains("type")) {
        throw std::runtime_error("Malformed error response. Could not deduce response type.");
    }

    if (json["type"] != "message") {
        throw std::runtime_error("Malformed message response. Response is not a message");
    }

    MessagesResult ok;

    if (json["content"].empty()) {
        ok.output = "LLM returned no content.";
    } else {
        ok.output = json["content"][0]["text"];
    }

    ok.input_tokens = json["usage"]["input_tokens"];
    ok.llm_model = json["model"];
    ok.output_tokens = json["usage"]["output_tokens"];
    ok.raw_response = json.dump(4);
    ok.stop_reason = json["stop_reason"];
    return ok;
}

} // namespace

namespace api {

void MessagesInput::set_max_tokens(const int max_tokens)
{
    this->max_tokens_ = max_tokens;

    if (this->max_tokens_ < 1) {
        this->max_tokens_ = 1;
    }
}

void MessagesInput::set_llm_model(const std::string &model)
{
    if (model.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }

    this->llm_model_ = model;
}

void MessagesInput::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

void MessagesInput::append_assistant_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "assistant" }, { "content", content } });
}

std::string MessagesInput::get_post_fields() const
{
    const nlohmann::json json = {
        { "max_tokens", this->max_tokens_ },
        { "messages", this->conversation_ },
        { "model", this->llm_model_ }
    };
    return json.dump();
}

std::expected<MessagesResult, Err> CreateMessage::query_api(const MessagesInput &input)
{
    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + this->user_api_key_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, headers);

    const std::string post_fields = input.get_post_fields();
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
        return unpack_200_response_(response);
    }

    return std::unexpected(unpack_error(response));
}

} // namespace api
