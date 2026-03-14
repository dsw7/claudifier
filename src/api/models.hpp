#pragma once

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

// models for requests
// -------------------

struct ModelMessages {
    bool print_help_and_exit = false;
    bool print_raw_response = false;
    int token_limit = 1024;
    std::string llm_model = "claude-opus-4-6";
    std::string prompt;
    nlohmann::json conversation;

    void append_user_message(const std::string &content);
};

struct ModelListModels {
    bool print_help_and_exit = false;
    int limit = 1000;
};

// models for responses
// -------------------

struct Err {
    std::string errmsg;
    std::string raw_response;
    std::string error_type;
};

struct ModelMessagesResult {
    int input_tokens = 0;
    int output_tokens = 0;
    std::string llm_model;
    std::string output;
    std::string raw_response;
};

struct ModelData {
    std::string created_at;
    std::string display_name;
    std::string id;
};

struct ModelListModelsResult {
    bool has_more = false;
    std::string last_id;
    std::string raw_response;
    std::vector<ModelData> data;

    void add_data(const std::string &created_at, const std::string &display_name, const std::string &id)
    {
        this->data.push_back(ModelData { created_at, display_name, id });
    }
};
