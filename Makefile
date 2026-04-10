BUILD_DIR = build
SRC_DIR = csarian

build_linux:
	@gcc -o build/sarian \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/error_handling/error.c \
	$(SRC_DIR)/lexer.c \
	$(SRC_DIR)/debug/debug.c \
	$(SRC_DIR)/interpreter.c \
	$(SRC_DIR)/functions/fn.c \
	$(SRC_DIR)/binary_operations/binary_operations.c \
	$(SRC_DIR)/global_variables/global_vars.c \
	$(SRC_DIR)/comparison/comparison.c \
	$(SRC_DIR)/token_utils/token_utils.c \
	$(SRC_DIR)/labels/label.c \
	$(SRC_DIR)/local_variables/local_vars.c \
	-lm

	@echo "Build Finished."

# Format all .c and .h files
format:
	@find $(SRC_DIR) -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format-17 -i {} +
	@echo "All source files formatted."

help:
	@echo "Available Builds:"
	@echo "Linux: make build_linux"