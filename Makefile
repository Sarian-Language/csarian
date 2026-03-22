BUILD_DIR = build
SRC_DIR = csarian

compile:
	gcc -o $(BUILD_DIR)/sarian $(SRC_DIR)/main.c $(SRC_DIR)/error.c $(SRC_DIR)/lexer.c $(SRC_DIR)/debug.c $(SRC_DIR)/interpreter.c -lm