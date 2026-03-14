from datetime import date
from json import loads
from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_root_help_messages(flag: str) -> None:
    process = run_claudifier(flag)
    assert process.exit_code == 0, process.stderr


@mark.parametrize("flag", ["-v", "--version"])
def test_version(flag: str) -> None:
    process = run_claudifier(flag)
    assert process.exit_code == 0, process.stderr

    output = loads(process.stdout)
    assert output["build_type"] == "Testing", "'build_type' is not 'Testing'"
    assert output["version"] == "1.0.0", "Version is not '1.0.0'"

    today = date.today().isoformat()
    build_date = output["build_date"].split("T")[0]
    assert build_date == today, f"Build date {build_date} is not today {today}"


def test_unknown_command() -> None:
    process = run_claudifier("foobar")
    assert process.exit_code == 1
    assert process.stderr == "Received unknown command. Re-run with -h or --help\n"
