#pragma once

#include <string>

struct ConfigsRun {
    std::string model = "claude-3-haiku-20240307";
};

struct ConfigsChat {
    std::string model = "claude-3-haiku-20240307";
};

struct Configs {
    ConfigsRun configs_run;
    ConfigsChat configs_chat;
};
