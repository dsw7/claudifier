#include "models.hpp"

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
        { "max_tokens", this->token_limit },
        { "messages", this->conversation_ },
        { "model", this->llm_model_ }
    };
    return json.dump();
}
