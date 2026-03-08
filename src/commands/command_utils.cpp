#include "command_utils.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <thread>

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
