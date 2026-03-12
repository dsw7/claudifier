from helpers import run_claudifier


def test_root_help_short() -> None:
    process = run_claudifier("-h")
    assert process.exit_code == 0, process.stderr


def test_root_help_long() -> None:
    process = run_claudifier("--help")
    assert process.exit_code == 0, process.stderr
