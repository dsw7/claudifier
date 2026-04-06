#include "api_keys.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace {

std::string load_api_key_(const std::string &key_name)
{
    const char *api_key = std::getenv(key_name.c_str());

    if (api_key == nullptr) {
        throw std::runtime_error(fmt::format("Environment variable {} not found.", key_name));
    }

    return std::string(api_key);
}

} // namespace

namespace api {

std::string get_user_api_key()
{
    static std::string api_key = load_api_key_("ANTHROPIC_API_KEY");
    return api_key;
}

} // namespace api
