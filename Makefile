.PHONY = format compile clean lint
.DEFAULT_GOAL = compile

BUILD_DIR = build

format:
	@clang-format -i --verbose --style=file src/*.cpp src/*.hpp

compile: format
	@cmake -S src -B $(BUILD_DIR)
	@make --jobs=12 --directory=$(BUILD_DIR)

clean:
	@rm -rfv $(BUILD_DIR)

lint:
	@cppcheck src --enable=all
