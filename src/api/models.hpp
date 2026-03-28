#pragma once

#include <string>

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
