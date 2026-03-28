#pragma once

#include <string>

namespace api {

struct Err {
    std::string errmsg;
    std::string raw_response;
    std::string error_type;
};

Err unpack_error(const std::string &response);
} // namespace api
