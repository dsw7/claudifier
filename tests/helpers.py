from dataclasses import dataclass
from functools import cache
from os import environ
from subprocess import run, PIPE
import pytest


@dataclass
class Results:
    stdout: str
    stderr: str
    exit_code: int


@cache
def _load_path_to_binary() -> str:
    if "PATH_BIN" not in environ:
        pytest.exit(
            reason="Aborting pytest session: 'PATH_BIN' is not set in environment variables."
        )

    return environ["PATH_BIN"]


def run_claudifier(*cli_args: str) -> Results:
    command = [_load_path_to_binary()]
    command.extend(cli_args)

    result = run(command, stdout=PIPE, stderr=PIPE, text=True)

    return Results(
        stdout=result.stdout, stderr=result.stderr, exit_code=result.returncode
    )
