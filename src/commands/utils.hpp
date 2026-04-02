#pragma once

#include <atomic>
#include <filesystem>
#include <fmt/color.h>
#include <string>

namespace colors {
constexpr inline fmt::terminal_color green = fmt::terminal_color::bright_green;
constexpr inline fmt::terminal_color yellow = fmt::terminal_color::bright_yellow;
constexpr inline fmt::terminal_color red = fmt::terminal_color::bright_red;
} // namespace colors

namespace utils {
void print_line();
std::string read_input_from_stdin();
int string_to_int(const std::string &str);
float string_to_float(const std::string &str);
std::string read_from_file(const std::filesystem::path &filepath);
} // namespace utils

namespace threading {
extern std::atomic<bool> timer_enabled;
void time_api_call();
} // namespace threading
