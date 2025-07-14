.PHONY = format compile tidy clean lint
.DEFAULT_GOAL = compile

BUILD_DIR = build

format:
	@clang-format -i --verbose --style=file src/*.cpp src/*.hpp

compile: format
	@cmake -S src -B $(BUILD_DIR)
	@make --jobs=12 --directory=$(BUILD_DIR) install

tidy:
	@cmake -S src -B $(BUILD_DIR)
	@clang-tidy -p $(BUILD_DIR) src/*.cpp src/*.hpp

clean:
	@rm -rfv $(BUILD_DIR)

lint:
	@cppcheck src --enable=all
