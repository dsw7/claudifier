#pragma once

#include <string>

struct Err {
    std::string errmsg;
    std::string raw_response;
    std::string error_type;
};
