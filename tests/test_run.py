from json import loads
from pytest import mark
from helpers import run_claudifier


@mark.parametrize("flag", ["-h", "--help"])
def test_help_messages(flag: str) -> None:
    process = run_claudifier("run", flag)
    assert process.exit_code == 0, process.stderr


@mark.parametrize("arg", ["-p", "--prompt="])
def test_prompt_arg(arg: str) -> None:
    process = run_claudifier("run", f"{arg}What is 3 + 5? Return just the result")

    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)
    assert results["output"] == "8"


@mark.parametrize("arg", ["-m", "--model="])
def test_model_arg(arg: str) -> None:
    process = run_claudifier(
        "run", "--prompt=Running a test. Please ignore me", f"{arg}claude-opus-4-6"
    )

    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)
    assert results["llm_model"] == "claude-opus-4-6"


@mark.parametrize("arg", ["-l", "--limit="])
def test_limit_arg(arg: str) -> None:
    process = run_claudifier(
        "run", "--prompt=Running a test. Please ignore me", f"{arg}2"
    )

    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)
    assert results["output_tokens"] == 2


@mark.parametrize("arg", ["-r", "--raw"])
def test_raw_response_arg(arg: str) -> None:
    process = run_claudifier("run", "--prompt=Running a test. Please ignore me", arg)

    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)

    assert results["type"] == "message"
    assert results["stop_reason"] == "end_turn"


def test_invalid_model() -> None:
    process = run_claudifier(
        "run", "--prompt=Running a test. Please ignore me", "--model=foobar"
    )
    assert process.exit_code == 1
    assert (
        process.stderr == "An error occurred when creating message: 'model: foobar'\n"
    )


def test_no_content_handling() -> None:
    process = run_claudifier(
        "run", "--prompt=What is 3 + 5? Return just the result", "--limit=1"
    )
    assert process.exit_code == 1
    assert (
        process.stderr
        == "Failed to create message: 'Malformed message response. Content is empty'\n"
    )


def test_empty_limit() -> None:
    process = run_claudifier(
        "run", "--prompt=Running a test. Please ignore me", "--limit="
    )
    assert process.exit_code == 1
    assert process.stderr == "Cannot convert string to int. Input string is empty\n"


def test_empty_model() -> None:
    process = run_claudifier(
        "run", "--prompt=Running a test. Please ignore me", "--model="
    )
    assert process.exit_code == 1
    assert process.stderr == "The model parameter is empty\n"


def test_empty_prompt() -> None:
    process = run_claudifier("run", "--prompt=")
    assert process.exit_code == 1
    assert process.stderr == "The prompt is empty\n"
