#pragma once

#include <atomic>
#include <string>

namespace utils {
void print_line();
std::string read_input_from_stdin();
int string_to_int(const std::string &str);
float string_to_float(const std::string &str);
} // namespace utils

namespace threading {
extern std::atomic<bool> timer_enabled;
void time_api_call();
} // namespace threading
