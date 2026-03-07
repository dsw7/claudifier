#pragma once

#include <json.hpp>
#include <string>

namespace utils {
nlohmann::json parse_json(const std::string &response);
}
