#pragma once

#include <json.hpp>
#include <string>

namespace api {

class Err {
public:
    Err(const std::string &response);
    std::string errmsg;
    std::string raw_response;
    std::string error_type;

private:
    nlohmann::json response_;
    void validate_schema_();
};

} // namespace api
