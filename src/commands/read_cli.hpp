#pragma once

#include <string>

namespace commands {

struct ParamsRun {
    bool print_help_and_exit = false;
    std::string model = "claude-opus-4-6";
    std::string prompt;
};

void read_cli(int argc, char **argv, ParamsRun &params);
} // namespace commands
