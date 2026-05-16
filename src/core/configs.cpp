#include "configs.hpp"

#include "datadir.hpp"

#include <stdexcept>
#include <toml.hpp>

Configs::Configs()
{
    const std::filesystem::path proj_config = datadir::get_config_file();
    toml::table table;

    try {
        table = toml::parse_file(proj_config.string());
    } catch (const toml::parse_error &e) {
        throw std::runtime_error(e);
    }

    configs_run.model = table["run-command"]["model"].value_or("claude-haiku-4-5");
    configs_run.temperature = table["run-command"]["temperature"].value_or(1.0f);
    configs_run.max_tokens = table["run-command"]["max_tokens"].value_or(1024);

    configs_chat.model = table["chat-command"]["model"].value_or("claude-haiku-4-5");
    configs_chat.temperature = table["chat-command"]["temperature"].value_or(1.0f);
    configs_chat.max_tokens = table["chat-command"]["max_tokens"].value_or(4096);
}
