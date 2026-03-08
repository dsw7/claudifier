#include "command_utils.hpp"

#include <array>
#include <chrono>
#include <fmt/core.h>
#include <iostream>
#include <thread>

namespace {

constexpr std::string get_line_()
{
    std::string line;

    for (int i = 0; i < 100; i++) {
        line += "\u2500";
    }

    return line;
}

} // namespace

void print_line()
{
    fmt::print("{}\n", get_line_());
}

namespace threading {

std::atomic<bool> timer_enabled(false);

void time_api_call()
{
    static std::chrono::duration delay = std::chrono::milliseconds(100);
    static std::array spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    static int num_frames = spinner.size();

    while (timer_enabled.load()) {
        for (int i = 0; i < num_frames; ++i) {
            std::cout << "\r" << spinner[i] << std::flush;
            std::this_thread::sleep_for(delay);
        }
    }

    std::cout << " \r" << std::flush;
}

} // namespace threading
