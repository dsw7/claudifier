from json import loads
from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("models", flag)
    assert process.exit_code == 0, process.stderr


def test_exit_zero() -> None:
    process = run_claudifier("models")
    assert process.exit_code == 0, process.stderr


def test_json_dump() -> None:
    process = run_claudifier("models", "--json")
    assert process.exit_code == 0, process.stderr

    stdout = loads(process.stdout)
    assert "models" in stdout
    assert not stdout["has_more"]
