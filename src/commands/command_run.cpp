#include "command_run.hpp"

#include "api.hpp"

#include <fmt/core.h>
#include <stdexcept>
#include <string>

namespace {

void create_message()
{
    api::Curl curl;
    const std::string model = "claude-opus-4-6";
    const auto result = curl.create_message("What is 3 + 5?", model);

    if (result) {
        fmt::print("{}\n", result->output);
    } else {
        throw std::runtime_error(result.error().errmsg);
    }
}
} // namespace

namespace commands {
void command_run()
{
    create_message();
}
} // namespace commands
