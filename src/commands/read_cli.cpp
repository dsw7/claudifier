#include "read_cli.hpp"

#include "command_utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>

namespace {

void help_run_command_()
{
    const std::string messages = R"(Create a message according to a prompt.
Messaging using the run command is stateless.
The prompt can be read in interactively or via command line argument.

Usage:
  claudifier run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -p, --prompt=PROMPT            Specify the prompt message
  -l, --limit=LIMIT              Specify token limit
)";

    fmt::print("{}\n", messages);
}

void help_models_command_()
{
    const std::string messages = R"(Get a list of available models.

Usage:
  claudifier models [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -l, --limit=LIMIT              Specify LIMIT number of models per page
)";

    fmt::print("{}\n", messages);
}

} // namespace

namespace commands {

void read_cli(int argc, char **argv, ModelMessages &model)
{
    bool print_help_and_exit = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:l:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_and_exit = true;
                break;
            case 'm':
                model.llm_model = optarg;
                break;
            case 'p':
                model.prompt = optarg;
                break;
            case 'l':
                model.token_limit = utils::string_to_int(optarg);
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (print_help_and_exit) {
        help_run_command_();
        exit(EXIT_SUCCESS);
    }
}

void read_cli(int argc, char **argv, ModelListModels &model)
{
    bool print_help_and_exit = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hl:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_and_exit = true;
                break;
            case 'l':
                model.limit = utils::string_to_int(optarg);
                break;
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} models [-h | --help] for more information", argv[0]));
        }
    };

    if (print_help_and_exit) {
        help_models_command_();
        exit(EXIT_SUCCESS);
    }
}

} // namespace commands
