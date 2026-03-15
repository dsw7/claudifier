#pragma once

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

// models for requests
// -------------------

class ModelMessages {
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

class ModelListModels {
public:
    void set_max_items_per_page(const int limit);
    int get_max_items_per_page() const;

private:
    int items_per_page_ = 1000;
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
    std::string stop_reason;
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
