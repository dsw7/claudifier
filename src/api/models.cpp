#include "models.hpp"

void ModelMessages::set_max_tokens(const int max_tokens)
{
    this->max_tokens_ = max_tokens;

    if (this->max_tokens_ < 1) {
        this->max_tokens_ = 1;
    }
}

void ModelMessages::set_llm_model(const std::string &model)
{
    if (model.empty()) {
        throw std::runtime_error("The model parameter is empty");
    }

    this->llm_model_ = model;
}

void ModelMessages::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

std::string ModelMessages::get_post_fields() const
{
    const nlohmann::json json = {
        { "max_tokens", this->max_tokens_ },
        { "messages", this->conversation_ },
        { "model", this->llm_model_ }
    };
    return json.dump();
}
