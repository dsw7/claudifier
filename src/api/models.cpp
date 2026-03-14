#include "models.hpp"

void ModelMessages::append_user_message(const std::string &content)
{
    this->conversation.push_back({ { "role", "user" }, { "content", content } });
}
