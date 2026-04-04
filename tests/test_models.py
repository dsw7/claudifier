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


def test_json_dump_and_correct_page_numbers() -> None:
    process = run_claudifier("models", "--json", "--limit=4")
    assert process.exit_code == 0
    pages = [i["page"] for i in loads(process.stdout)]

    assert len(pages) > 8, "Test cannot proceed - too few models"
    assert pages.count(1) == 4
    assert pages.count(2) == 4
