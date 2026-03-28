#include "models.hpp"

#include <algorithm>

// models for requests
// -------------------

void ListModels::set_max_items_per_page(const int limit)
{
    static int min_models_per_page = 1;
    static int max_models_per_page = 1000;
    this->items_per_page_ = std::clamp(limit, min_models_per_page, max_models_per_page);
}

int ListModels::get_max_items_per_page() const
{
    return this->items_per_page_;
}

// models for responses
// -------------------

void ListModelsPage::append_llm_model_to_page(const std::string &created_at, const std::string &display_name, const std::string &id)
{
    this->data.push_back(ModelData { created_at, display_name, id });
}
