#include "command_chat.hpp"

#include "command_utils.hpp"
#include "query_messages_api.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>

namespace {

void print_help_messages_()
{
    const std::string messages = R"(Participate in a multi-turn conversation with LLM.

Usage:
  claudifier chat [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -l, --limit=LIMIT              Specify token limit
)";

    fmt::print("{}\n", messages);
}

void preprocess_and_validate_params_(ModelMessages &model)
{
    if (model.token_limit < 1) {
        model.token_limit = 1;
    }
}

ModelMessages read_cli_(const int argc, char **argv)
{
    ModelMessages model;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:l:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages_();
                exit(EXIT_SUCCESS);
            case 'm':
                model.set_llm_model(optarg);
                break;
            case 'l':
                model.token_limit = utils::string_to_int(optarg);
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    preprocess_and_validate_params_(model);
    return model;
}

// ----------------------------------------------------------------------------------------------------------
} // namespace

namespace commands {

void command_chat(const int argc, char **argv)
{
    const ModelMessages model = read_cli_(argc, argv);
}

} // namespace commands
