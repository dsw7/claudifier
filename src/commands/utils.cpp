#include "utils.hpp"

#include <array>
#include <chrono>
#include <fmt/core.h>
#include <iostream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

namespace {

std::string get_line_()
{
    std::string line;
    int width = 100;

    if (isatty(STDOUT_FILENO)) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
    }

    for (int i = 0; i < width; i++) {
        line += "\u2500";
    }

    return line;
}

} // namespace

namespace utils {

void print_line()
{
    static std::string line = get_line_();
    fmt::print("{}\n", line);
}

std::string read_input_from_stdin()
{
    std::string input;
    while (true) {
        fmt::print(fmt::emphasis::bold, "Input: ");
        std::getline(std::cin, input);

        if (input.empty()) {
            fmt::print("Input is empty. Try again.\n");
        } else {
            break;
        }
    }

    return input;
}

int string_to_int(const std::string &str)
{
    if (str.empty()) {
        throw std::runtime_error("Cannot convert string to int. Input string is empty");
    }

    int i = 0;

    try {
        i = std::stoi(str);
    } catch (const std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to int", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return i;
}

float string_to_float(const std::string &str)
{
    if (str.empty()) {
        throw std::runtime_error("Cannot convert string to float. Input string is empty");
    }

    float f = 0.00;

    try {
        f = std::stof(str);
    } catch (const std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return f;
}

} // namespace utils

namespace threading {

std::atomic<bool> timer_enabled(false);

void time_api_call()
{
    static std::chrono::duration delay = std::chrono::milliseconds(100);
    static std::array spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    static int num_frames = spinner.size();

#ifdef TESTING_ENABLED
    // mock everything but actually printing to stdout since it
    // interferes with JSON parsing in unit tests
    while (timer_enabled.load()) {
        for (int i = 0; i < num_frames; ++i) {
            std::this_thread::sleep_for(delay);
        }
    }
#else
    while (timer_enabled.load()) {
        for (int i = 0; i < num_frames; ++i) {
            std::cout << "\r" << spinner[i] << std::flush;
            std::this_thread::sleep_for(delay);
        }
    }
    std::cout << " \r" << std::flush;
#endif
}

} // namespace threading
