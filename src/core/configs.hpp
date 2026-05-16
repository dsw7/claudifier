#pragma once

#include <string>

struct ConfigsRun {
    float temperature = 1.0f;
    int max_tokens = 1024;
    std::string model = "claude-haiku-4-5";
};

struct ConfigsChat {
    float temperature = 1.0f;
    int max_tokens = 4096;
    std::string model = "claude-haiku-4-5";
};

struct Configs {
    ConfigsRun configs_run;
    ConfigsChat configs_chat;
};
