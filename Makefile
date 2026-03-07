.PHONY = format compile clean

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod

.DEFAULT_GOAL = compile

format:
	@clang-format -i --verbose --style=file src/*.cpp src/*.hpp src/api/*.cpp src/api/*.hpp

compile: format
	@cmake -S src -B $(BUILD_DIR_PROD)
	@make --jobs=12 --directory=$(BUILD_DIR_PROD)

clean:
	@rm -rfv $(BUILD_DIR)
