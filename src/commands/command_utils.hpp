#pragma once

#include <atomic>
#include <string>

namespace utils {
void print_line();
int string_to_int(const std::string &str);
} // namespace utils

namespace threading {
extern std::atomic<bool> timer_enabled;
void time_api_call();
} // namespace threading
