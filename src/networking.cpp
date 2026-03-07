#include "networking.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace {

std::string load_user_api_key_()
{
    const char *api_key = std::getenv("ANTHROPIC_API_KEY");

    if (api_key == nullptr) {
        throw std::runtime_error("Environment variable ANTHROPIC_API_KEY not found.");
    }

    return std::string(api_key);
}

std::string get_user_api_key_()
{
    static std::string api_key = load_user_api_key_();
    return api_key;
}

} // namespace

namespace networking {

void create_message()
{
}

} // namespace networking
