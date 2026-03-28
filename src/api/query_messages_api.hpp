#pragma once

#include "curl_base.hpp"
#include "models.hpp"

#include <string>

namespace api {

class MessagesInput {
public:
    bool print_raw_response = false;

    void set_max_tokens(const int max_tokens);
    void set_llm_model(const std::string &model);
    void append_user_message(const std::string &content);
    void append_assistant_message(const std::string &content);
    std::string get_post_fields() const;

private:
    int max_tokens_ = 1024;
    nlohmann::json conversation_;
    std::string llm_model_ = "claude-opus-4-6";
};

class CreateMessage: public CurlBase {
public:
    std::expected<MessagesResult, Err> query_api(const MessagesInput &input);
};

} // namespace api
