#pragma once

#include <expected>
#include <string>

struct Err {
    long code = -1;
    std::string message;
};

struct OkMessage {
    int input_tokens = 0;
    int output_tokens = 0;
    std::string model;
    std::string output;
    std::string raw_response;
};

using MessageResult = std::expected<OkMessage, Err>;
