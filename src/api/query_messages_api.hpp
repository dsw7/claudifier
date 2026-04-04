#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <json.hpp>
#include <optional>
#include <string>

namespace api {

class MessagesOutput {
public:
    MessagesOutput() = default;
    MessagesOutput(const std::string &response);
    std::string get_latest_text() const;

    double rtt_time = 0;
    float temperature = 1.0f;
    int input_tokens = 0;
    int output_tokens = 0;
    std::string llm_model;
    std::string raw_response;
    std::string stop_reason;

private:
    nlohmann::json response_;
    void validate_schema_();
};

class CreateMessage: public CurlBase {
public:
    void set_max_tokens(const int max_tokens);
    void set_temperature(const float temperature);
    void set_llm_model(const std::string &model);
    void set_system_prompt(const std::string &prompt);
    void append_user_message(const std::string &content);
    void append_assistant_message(const std::string &content);
    nlohmann::json get_conversation() const;
    std::expected<MessagesOutput, Err> query_api();

private:
    float temperature_ = 1.0f;
    int max_tokens_ = 1024;
    nlohmann::json conversation_;
    std::optional<std::string> system_prompt_;
    std::string llm_model_ = "claude-3-haiku-20240307";
};

} // namespace api
