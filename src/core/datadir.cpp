#include "datadir.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace {

std::filesystem::path get_home_dir_()
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir) {
        throw std::runtime_error("Could not locate user home directory!");
    }

    return std::filesystem::path(home_dir);
}

std::filesystem::path get_project_dir_()
{
    static std::filesystem::path proj_dir = get_home_dir_() / ".claudifier";

    if (not std::filesystem::exists(proj_dir)) {
        throw std::runtime_error(fmt::format("Could not locate '{}'", proj_dir.string()));
    }

    return proj_dir;
}

} // namespace

namespace datadir {

std::filesystem::path get_config_file()
{
    static std::filesystem::path config_file = get_project_dir_() / "claudifier.toml";

    if (not std::filesystem::exists(config_file)) {
        throw std::runtime_error("Could not locate Claudifier configuration file!");
    }

    return config_file;
}

std::filesystem::path get_completions_dir()
{
    static std::filesystem::path dir_completions = get_project_dir_() / "completions";

    if (not std::filesystem::exists(dir_completions)) {
        std::filesystem::create_directory(dir_completions);
    }

    return dir_completions;
}

} // namespace datadir
