from pytest import mark
from helpers import run_claudifier


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
