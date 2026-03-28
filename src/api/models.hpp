#pragma once

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

// models for requests
// -------------------

class ListModels {
public:
    int get_max_items_per_page() const;
    void set_max_items_per_page(const int limit);
    bool dump_json = false;

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

struct ModelData {
    std::string created_at;
    std::string display_name;
    std::string id;
};

struct ListModelsPage {
    bool has_more = false;
    std::string last_id;
    std::string raw_response;
    std::vector<ModelData> data;

    void append_llm_model_to_page(const std::string &created_at, const std::string &display_name, const std::string &id);
};
