from json import loads
from pytest import mark
from helpers import run_claudifier


def test_chat() -> None:
    process = run_claudifier("test", "--chat")
    assert process.exit_code == 0, process.stderr
    conversation = loads(process.stdout)

    assert len(conversation) == 4
    assert conversation[0]["role"] == "user"
    assert conversation[1]["role"] == "assistant"
    assert conversation[2]["role"] == "user"
    assert conversation[3]["role"] == "assistant"

    assert (
        conversation[0]["content"] == "If a = 2, b = 3, and c = a + b, then what is c?"
    )
    assert conversation[2]["content"] == "What is c + 5? Return just the value"
    assert conversation[3]["content"] == "10"


@mark.skip(reason="Need Valgrind infrastructure")
def test_zero_shot() -> None:
    process = run_claudifier("test", "--zero-shot")
    assert process.exit_code == 0, process.stderr


@mark.skip(reason="Need Valgrind infrastructure")
def test_one_shot() -> None:
    process = run_claudifier("test", "--one-shot")
    assert process.exit_code == 0, process.stderr


@mark.skip(reason="Need Valgrind infrastructure")
def test_few_shot() -> None:
    process = run_claudifier("test", "--few-shot")
    assert process.exit_code == 0, process.stderr


@mark.skip(reason="Need Valgrind infrastructure")
def test_chain_of_thought() -> None:
    process = run_claudifier("test", "--chain-of-thought")
    assert process.exit_code == 0, process.stderr


@mark.skip(reason="Need Valgrind infrastructure")
def test_tree_of_thought() -> None:
    process = run_claudifier("test", "--tree-of-thought")
    assert process.exit_code == 0, process.stderr
