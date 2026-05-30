#pragma once

#include <filesystem>

namespace datadir {
std::filesystem::path get_config_file();
std::filesystem::path get_completions_dir();
} // namespace datadir
