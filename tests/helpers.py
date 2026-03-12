from subprocess import run, PIPE
from dataclasses import dataclass


@dataclass
class Results:
    stdout: str
    stderr: str
    exit_code: int


def run_claudifier(*cli_args: str) -> Results:
    command = ["./build/test/claudifier"]
    command.extend(cli_args)

    result = run(command, stdout=PIPE, stderr=PIPE, text=True)

    return Results(
        stdout=result.stdout, stderr=result.stderr, exit_code=result.returncode
    )
