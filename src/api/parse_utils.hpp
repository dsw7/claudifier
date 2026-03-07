#pragma once

#include <json.hpp>
#include <string>

namespace api {
nlohmann::json parse_response(const std::string &response);
std::string unpack_error(const std::string &response);
} // namespace api
