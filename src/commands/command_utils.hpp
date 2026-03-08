#pragma once

#include <atomic>

void print_line();

namespace threading {
extern std::atomic<bool> timer_enabled;
void time_api_call();
} // namespace threading
