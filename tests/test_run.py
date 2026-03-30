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
    assert results["stop_reason"] == "max_tokens"


def test_invalid_limit() -> None:
    process = run_claudifier("run", "-pHello", "--limit=abc")
    assert process.exit_code == 1
    assert process.stderr == "stoi\nFailed to convert 'abc' to int\n"


def test_out_of_range_limit() -> None:
    process = run_claudifier("run", "--prompt=What is 3 + 5?", "--limit=-5")
    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)
    # clamps to 1 token which is not enough to return any content
    assert results["output"] == "LLM returned no content."
    assert results["output_tokens"] == 1


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
    assert process.exit_code == 0, process.stderr
    results = loads(process.stdout)
    assert results["output"] == "LLM returned no content."


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


@mark.skip(reason="Test will always hang as empty prompt forces read from stdin")
def test_empty_prompt() -> None:
    process = run_claudifier("run", "--prompt=")
    assert process.exit_code == 1
    assert process.stderr == "The prompt is empty\n"


@mark.parametrize(
    "prompt, diverted", [("What is 2 + 2?", False), ("Is Arizona in the USA?", True)]
)
def test_system_prompt(prompt: str, diverted: bool) -> None:
    process = run_claudifier(
        "run",
        "--raw",
        f"--prompt={prompt}",
        "--system=You are a math teacher. Ensure the student only asks questions related to "
        'math. Return the response in valid JSON of form {"result": <string>, "diverted": <bool>}. '
        'Set the "diverted" to true in the JSON if you have to divert the conversation.',
    )
    assert process.exit_code == 0
    stdout = loads(process.stdout)
    text = loads(stdout["content"][0]["text"])
    assert text["diverted"] == diverted


def test_system_prompt_empty() -> None:
    process = run_claudifier("run", "--prompt=What is 3 + 2?", "--system=")
    assert process.exit_code == 1
    assert process.stderr == "The provided system prompt is empty\n"


@mark.parametrize("input_temp, clamped_temp", [(-0.4, 0), (0.5, 0.5), (100, 1)])
def test_temperature(input_temp: float, clamped_temp: float) -> None:
    process = run_claudifier(
        "run", f"--temperature={input_temp}", "--prompt=What is 3 + 5?"
    )
    assert process.exit_code == 0
    stdout = loads(process.stdout)
    assert float(stdout["temperature"]) == clamped_temp


def test_temperature_short_opt() -> None:
    process = run_claudifier("run", "-t0.5", "--prompt=What is 3 + 5?")
    assert process.exit_code == 0
    stdout = loads(process.stdout)
    assert float(stdout["temperature"]) == 0.5
