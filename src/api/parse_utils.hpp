#pragma once

#include "responses.hpp"

#include <json.hpp>
#include <string>

namespace api {
nlohmann::json parse_response(const std::string &response);
Err unpack_error(const std::string &response);
} // namespace api
