#include "read_cli.hpp"

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
  cl run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid messages model
  -p, --prompt=PROMPT            Specify the prompt message
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
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:p:", long_options, &option_index);

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
            default:
                throw std::runtime_error(fmt::format("Unknown argument. Try running {} run [-h | --help] for more information", argv[0]));
        }
    };

    if (print_help_and_exit) {
        help_run_command_();
        exit(EXIT_SUCCESS);
    }
}

} // namespace commands
