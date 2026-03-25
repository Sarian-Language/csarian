BUILD_DIR = build
SRC_DIR = csarian

build_linux:
	gcc -o $(BUILD_DIR)/sarian $(SRC_DIR)/main.c $(SRC_DIR)/error.c \
	$(SRC_DIR)/lexer.c $(SRC_DIR)/debug/debug.c $(SRC_DIR)/interpreter.c -lm

help:
	@echo "Available Builds:"
	@echo "Linux: make build_linux"