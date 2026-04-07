from json import loads
from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("costs", flag)
    assert process.exit_code == 0, process.stderr


def test_exit_zero() -> None:
    process = run_claudifier("costs")
    assert process.exit_code == 0, process.stderr
    assert "Overall usage (USD):" in process.stdout


@mark.parametrize("days, days_clamped", [(-5, 1), (5, 5), (100, 30)])
def test_days(days: int, days_clamped: int) -> None:
    process = run_claudifier("costs", "--json", f"--days={days}")
    assert process.exit_code == 0, process.stderr

    stdout = loads(process.stdout)
    assert "data" in stdout
    data = stdout["data"]

    assert len(data) == days_clamped
    assert not stdout["has_more"]
    assert stdout["next_page"] is None
