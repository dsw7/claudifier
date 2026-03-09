#pragma once

#include <string>

struct ModelMessages {
    bool print_help_and_exit = false;
    std::string llm_model = "claude-opus-4-6";
    std::string prompt;
};
