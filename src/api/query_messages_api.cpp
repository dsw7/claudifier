#include "query_messages_api.hpp"

#include "errors.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <stdexcept>
#include <vector>

namespace api {

CreateMessage::CreateMessage(
    const int max_tokens,
    const float temperature,
    const std::string &model,
    const std::optional<std::string> &system_prompt) :
    max_tokens_(max_tokens),
    temperature_(temperature),
    model_(model),
    system_prompt_(system_prompt)
{
    if (this->max_tokens_ < 1) {
        this->max_tokens_ = 1;
    }

    this->temperature_ = std::clamp(temperature, 0.0f, 1.0f);

    if (this->model_.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }

    if (this->system_prompt_) {
        if (this->system_prompt_->empty()) {
            throw std::runtime_error("The provided system prompt is empty");
        }
    }
}

void CreateMessage::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

void CreateMessage::append_assistant_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "assistant" }, { "content", content } });
}

nlohmann::json CreateMessage::get_conversation() const
{
    return this->conversation_;
}

std::expected<MessagesOutput, Err> CreateMessage::query_api()
{
    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + this->user_api_key_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, headers);

    nlohmann::json json = {
        { "max_tokens", this->max_tokens_ },
        { "messages", this->conversation_ },
        { "model", this->model_ },
        { "temperature", this->temperature_ }
    };
    if (this->system_prompt_) {
        json["system"] = this->system_prompt_;
    }
    const std::string post_fields = json.dump();
    curl_easy_setopt(this->curl_, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &response);

    const CURLcode code_easy_perform = curl_easy_perform(this->curl_);

    curl_slist_free_all(headers);
    headers = nullptr;

    if (code_easy_perform != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code_easy_perform));
    }

    long http_status_code = -1;
    const CURLcode code_info_resp = curl_easy_getinfo(this->curl_, CURLINFO_RESPONSE_CODE, &http_status_code);
    if (code_info_resp != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code_info_resp));
    }

    if (http_status_code == 200) {
        MessagesOutput output(response);
        output.temperature = this->temperature_;

        const CURLcode code_info_time = curl_easy_getinfo(this->curl_, CURLINFO_TOTAL_TIME, &output.rtt_time);
        if (code_info_time != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(code_info_time));
        }

        return output;
    }

    return std::unexpected(unpack_error(response));
}

void MessagesOutput::validate_schema_()
{
    if (this->response_.contains("type")) {
        if (this->response_["type"] != "message") {
            throw std::runtime_error("Malformed message response. Response is not a message");
        }
    } else {
        throw std::runtime_error("Malformed error response. Could not deduce response type.");
    }

    if (this->response_["content"].empty()) {
        throw std::runtime_error("LLM returned no content.");
    }
}

MessagesOutput::MessagesOutput(const std::string &response)
{
    try {
        this->response_ = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    this->validate_schema_();

    this->input_tokens = this->response_["usage"]["input_tokens"];
    this->llm_model = this->response_["model"];
    this->output_tokens = this->response_["usage"]["output_tokens"];
    this->raw_response = this->response_.dump(4);
    this->stop_reason = this->response_["stop_reason"];
}

std::string MessagesOutput::get_latest_text() const
{
    // helper function for getting the last message in a chain of TextBlocks
    std::vector<std::string> text_blocks;

    for (const auto &block: this->response_["content"]) {
        if (block["type"] == "text") {
            text_blocks.emplace_back(block["text"]);
        }
    }

    if (text_blocks.empty()) {
        throw std::runtime_error("No TextBlocks could be found");
    }

    return text_blocks.back();
}

} // namespace api
