#include "models.hpp"

void ModelMessages::append_user_message(const std::string &content)
{
    this->conversation_.push_back({ { "role", "user" }, { "content", content } });
}

std::string ModelMessages::get_post_fields() const
{
    const nlohmann::json json = {
        { "max_tokens", this->token_limit },
        { "messages", this->conversation_ },
        { "model", this->llm_model }
    };
    return json.dump();
}
