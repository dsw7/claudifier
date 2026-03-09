#pragma once

#include <expected>
#include <string>

// models for requests
// -------------------

struct ModelMessages {
    std::string llm_model = "claude-opus-4-6";
    std::string prompt;
};

// models for responses
// -------------------

struct Err {
    std::string errmsg;
    std::string raw_response;
    std::string error_type;
};

struct ModelMessagesResult {
    int input_tokens = 0;
    int output_tokens = 0;
    std::string llm_model;
    std::string output;
    std::string raw_response;
};
