# Claudifier

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/claudifier/master/LICENSE)

## Overview
**Claudifier** is a command line tool designed to interact with Claude's API.
The program supports a few operations out of the box, such as running stateless
queries, running a chatbot, and fetching current models. This program is
written purely in C++ because python just feels clunky and slow. This program
is tested on Ubuntu/Debian and macOS.

**Please note that I built this purely because I wanted a program with a very
small binary footprint. Anthropic already provides numerous CLI tools for
interacting with its API.**

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
  - [The `run` command](#the-run-command)
  - [The `models` command](#the-models-command)
  - [The `chat` command](#the-chat-command)
- [Administration](#administration)
  - [The `costs` command](#the-costs-command)
- [Uninstall Claudifier](#uninstall-claudifier)
- [License](#license)

## Installation

### Prerequisites
This program requires [CMake](https://cmake.org/), [{fmt}](https://fmt.dev/latest/) and
[libcurl](https://curl.se/libcurl/). These can be installed as follows:

#### Ubuntu/Debian
```console
apt install cmake libfmt-dev libcurl4-openssl-dev
```
#### macOS
```console
brew install cmake fmt
# libcurl usually comes bundled with macOS
```
#### Other systems
This program should work on other Unix-like systems (i.e. other Linux
distributions) however I do not extensively test these.

#### Set up environment variables
You will need a valid Anthropic API key. Set it as an environment variable:
```bash
export ANTHROPIC_API_KEY="<your-api-key>"
```

### Step 1: Compile the binary
Compile the binary by executing the `make` target:
```console
make
```
The binary will be installed into the directory specified by CMake's [install()
function](https://cmake.org/cmake/help/latest/command/install.html#command:install).
To clean up generated
artifacts:
```console
make clean
```

### Step 2: Test setup
Start the program:
```console
claudifier --help
```
The program will print a help message if it was set up correctly. You may get
some variation of:
```
-bash: claudifier: command not found
```
If so, try running `claudifier` in a new terminal window.

## Usage

### The `run` command

### The `chat` command

### The `models` command

## Administration

### The `costs` command

## Uninstall Claudifier
Verify that the `claudifier` command in your `$PATH` is the Claudifier binary
and not an alias for another application:
```console
claudifier -h
```
If confirmed, proceed to remove the binary with the following command:
```console
rm $(which claudifier)
```

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE)
file for details.
