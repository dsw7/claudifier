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
