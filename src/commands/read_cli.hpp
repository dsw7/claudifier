#pragma once

#include "models.hpp"

namespace commands {
void read_cli(const int argc, char **argv, ModelMessages &model);
void read_cli(const int argc, char **argv, ModelListModels &model);
} // namespace commands
