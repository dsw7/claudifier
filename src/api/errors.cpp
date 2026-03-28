#include "errors.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace api {

Err unpack_error(const std::string &response)
{
    nlohmann::json json;

    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    if (not json.contains("type")) {
        throw std::runtime_error("Malformed error response. Could not deduce response type.");
    }

    if (json["type"] != "error") {
        throw std::runtime_error("Malformed error response. Response is not an error");
    }

    Err e;
    e.errmsg = json["error"]["message"];
    e.error_type = json["error"]["type"];
    e.raw_response = json.dump(4);
    return e;
}

} // namespace api
