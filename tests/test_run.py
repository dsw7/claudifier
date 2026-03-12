from json import loads
from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("run", flag)
    assert process.exit_code == 0, process.stderr


@mark.parametrize("arg", ["-p", "--prompt"])
def test_run_command(arg: str) -> None:
    process = run_claudifier("run", f"{arg}=What is 3 + 5? Return just the result")
    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)

    assert results["output"] == "8"
