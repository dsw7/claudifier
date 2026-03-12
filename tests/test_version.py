from json import loads
from datetime import date
from pytest import fixture
from helpers import run_claudifier, Results


@fixture(scope="module")
def run_command() -> Results:
    return run_claudifier("--version")


def test_command_exists_with_zero_exit_code(run_command: Results) -> None:
    assert run_command.exit_code == 0, run_command.stderr


def test_build_type_is_testing(run_command: Results) -> None:
    output = loads(run_command.stdout)
    assert output["build_type"] == "Testing", "'build_type' is not 'Testing'"


def test_version_is_correct(run_command: Results) -> None:
    output = loads(run_command.stdout)
    assert output["version"] == "1.0.0", "Version is not '1.0.0'"


def test_date_matches_today(run_command: Results) -> None:
    output = loads(run_command.stdout)
    today = date.today().isoformat()
    build_date = output["build_date"].split("T")[0]
    assert build_date == today, f"Build date {build_date} is not today {today}"
