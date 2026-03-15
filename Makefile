.PHONY = format compile clean compile-test test py

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod
BUILD_DIR_TEST = $(BUILD_DIR)/test

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

compile-test: format
	@cmake -S src -B $(BUILD_DIR_TEST) -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON
	@make --jobs=12 --directory=$(BUILD_DIR_TEST)

test: export PATH_BIN = $(CURDIR)/$(BUILD_DIR_TEST)/claudifier
test: compile-test
	-@python3 -m pytest -vs tests
	@lcov --quiet --capture --directory=$(BUILD_DIR) --output-file $(BUILD_DIR)/coverage.info
	@lcov --quiet --remove $(BUILD_DIR)/coverage.info "/usr/*" "*/external/*" --output-file $(BUILD_DIR)/coverage.info
	@genhtml --quiet $(BUILD_DIR)/coverage.info --output-directory $(BUILD_DIR)/coverageResults
	@echo "See coverage report at: $(CURDIR)/$(BUILD_DIR)/coverageResults/index.html"

py:
	@black tests/*.py
	@pylint --exit-zero tests/*.py
	@mypy --strict tests/*.py
