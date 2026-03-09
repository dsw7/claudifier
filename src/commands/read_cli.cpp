#include "read_cli.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>

namespace commands {

void read_cli(int argc, char **argv, ModelMessages &model)
{
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
                model.print_help_and_exit = true;
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
}

} // namespace commands
