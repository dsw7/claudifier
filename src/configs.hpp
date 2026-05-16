#pragma once

#include <string>

struct ConfigsRun {
    float temperature = 1.0f;
    int max_tokens = 1024;
    std::string model = "claude-3-haiku-20240307";
};

struct ConfigsChat {
    std::string model = "claude-3-haiku-20240307";
};

struct Configs {
    ConfigsRun configs_run;
    ConfigsChat configs_chat;
};
