from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("chat", flag)
    assert process.exit_code == 0, process.stderr


def test_conversational_turn() -> None:
    process = run_claudifier("chat")
    assert process.exit_code == 0, process.stderr
    assert process.stdout == "**10**\n"
