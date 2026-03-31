#include "query_messages_api.hpp"

#include "errors.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <stdexcept>
#include <vector>

namespace api {

void MessagesInput::set_max_tokens(const int max_tokens)
{
    this->max_tokens_ = max_tokens;

    if (this->max_tokens_ < 1) {
        this->max_tokens_ = 1;
    }
}

void MessagesInput::set_temperature(const float temperature)
{
    static float min_temp = 0.0;
    static float max_temp = 1.0;
    this->temperature_ = std::clamp(temperature, min_temp, max_temp);
}

void MessagesInput::set_llm_model(const std::string &model)
{
    if (model.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }

    this->llm_model_ = model;
}

void MessagesInput::set_system_prompt(const std::string &prompt)
{
    if (prompt.empty()) {
        throw std::runtime_error("The provided system prompt is empty");
    }

    this->system_prompt_ = prompt;
}

void MessagesInput::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

void MessagesInput::append_assistant_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "assistant" }, { "content", content } });
}

float MessagesInput::get_temperature() const
{
    return this->temperature_;
}

nlohmann::json MessagesInput::get_conversation() const
{
    return this->conversation_;
}

std::string MessagesInput::get_post_fields() const
{
    nlohmann::json json = {
        { "max_tokens", this->max_tokens_ },
        { "messages", this->conversation_ },
        { "model", this->llm_model_ },
        { "temperature", this->temperature_ }
    };

    if (this->system_prompt_) {
        json["system"] = this->system_prompt_;
    }

    return json.dump();
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

std::expected<MessagesOutput, Err> CreateMessage::query_api(const MessagesInput &input)
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
        output.temperature = input.get_temperature();

        const CURLcode code_info_time = curl_easy_getinfo(this->curl_, CURLINFO_TOTAL_TIME, &output.rtt_time);
        if (code_info_time != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(code_info_time));
        }

        return output;
    }

    return std::unexpected(unpack_error(response));
}

} // namespace api
