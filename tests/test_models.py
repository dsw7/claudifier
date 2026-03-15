from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("models", flag)
    assert process.exit_code == 0, process.stderr


def test_exit_zero() -> None:
    process = run_claudifier("models")
    assert process.exit_code == 0, process.stderr


@mark.parametrize(
    "value", [-5, 20, 1005], ids=["Low limit", "Valid limit", "High limit"]
)
@mark.parametrize("arg", ["-l", "--limit="], ids=["Short argument", "Long argument"])
def test_limit_arg_with_clamping(arg: str, value: int) -> None:
    process = run_claudifier("models", f"{arg}{value}")
    assert process.exit_code == 0, process.stderr


def test_invalid_limit() -> None:
    process = run_claudifier("models", "--limit=abc")
    assert process.exit_code == 1
    assert process.stderr == "stoi\nFailed to convert 'abc' to int\n"


def test_empty_limit() -> None:
    process = run_claudifier("models", "--limit=")
    assert process.exit_code == 1
    assert process.stderr == "Cannot convert string to int. Input string is empty\n"
