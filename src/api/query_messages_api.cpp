#include "query_messages_api.hpp"

#include "api_keys.hpp"
#include "errors.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <stdexcept>
#include <vector>

namespace api {

CreateMessage::CreateMessage() :
    max_tokens_(1024),
    temperature_(1.0f),
    model_("claude-haiku-4-5") {}

CreateMessage::CreateMessage(
    const int max_tokens,
    const float temperature,
    const std::string &model) :
    max_tokens_(max_tokens),
    temperature_(temperature),
    model_(model)
{
    if (this->max_tokens_ < 1) {
        this->max_tokens_ = 1;
    }

    this->temperature_ = std::clamp(this->temperature_, 0.0f, 1.0f);

    if (this->model_.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }
}

void CreateMessage::set_system_prompt(const std::string &prompt)
{
    if (prompt.empty()) {
        throw std::runtime_error("The provided system prompt is empty");
    }

    this->system_prompt_ = prompt;
}

void CreateMessage::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

void CreateMessage::append_assistant_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "assistant" }, { "content", content } });
}

void CreateMessage::clear_conversation()
{
    this->conversation_.clear();
}

nlohmann::json CreateMessage::get_conversation() const
{
    return this->conversation_;
}

std::string CreateMessage::get_model() const
{
    return this->model_;
}

std::expected<MessagesOutput, Err> CreateMessage::query_api()
{
    curl_easy_setopt(this->curl_, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + get_user_api_key()).c_str());
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

    if (this->is_200_response_()) {
        MessagesOutput output(response);
        output.temperature = this->temperature_;
        output.rtt_time = this->get_rtt_time_();
        return output;
    }

    return std::unexpected(Err(response));
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
