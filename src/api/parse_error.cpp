#include "parse_error.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

nlohmann::json parse_json_(const std::string &response)
{
    nlohmann::json json;

    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    return json;
}

} // namespace

namespace api {

std::string parse_error(const std::string &response)
{
    const nlohmann::json json = parse_json_(response);

    if (not json.contains("error")) {
        throw std::runtime_error("Malformed error response. No error object could be found");
    }

    if (not json["error"].contains("message")) {
        throw std::runtime_error("Malformed error response. No message could be found");
    }

    return json["error"]["message"];
}

} // namespace api
