.PHONY = format compile clean py

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod

.DEFAULT_GOAL = compile

format:
	@clang-format -i --verbose --style=file src/*.cpp \
		src/api/*.cpp src/api/*.hpp \
		src/commands/*.cpp src/commands/*.hpp

compile: format
	@cmake -S src -B $(BUILD_DIR_PROD)
	@make --jobs=12 --directory=$(BUILD_DIR_PROD)

clean:
	@rm -rfv $(BUILD_DIR)

py:
	@black tests/*.py
	@pylint --exit-zero tests/*.py
	@mypy --strict tests/*.py
