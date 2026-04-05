#include "errors.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace api {

Err::Err(const std::string &response)
{
    try {
        this->response_ = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    this->validate_schema_();

    this->errmsg = this->response_["error"]["message"];
    this->error_type = this->response_["error"]["type"];
    this->raw_response = this->response_.dump(4);
}

void Err::validate_schema_()
{
    if (not this->response_.contains("type")) {
        throw std::runtime_error("Malformed error response. Could not deduce response type.");
    }

    if (this->response_["type"] != "error") {
        throw std::runtime_error("Malformed error response. Response is not an error");
    }
}

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
