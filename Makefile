BUILD_DIR = build
SRC_DIR = csarian

build_linux:
	@gcc -I. -o build/sarian \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/core/error_handling/error.c \
	$(SRC_DIR)/core/lexer/lexer.c \
	$(SRC_DIR)/core/interpreter/interpreter.c \
	$(SRC_DIR)/utils/debug/debug.c \
	$(SRC_DIR)/runtime/functions/fn.c \
	$(SRC_DIR)/expressions/binary_operations/binary_operations.c \
	$(SRC_DIR)/runtime/variables/global_variables/global_vars.c \
	$(SRC_DIR)/expressions/comparison/comparison.c \
	$(SRC_DIR)/utils/token_utils/token_utils.c \
	$(SRC_DIR)/runtime/labels/label.c \
	$(SRC_DIR)/runtime/variables/local_variables/local_vars.c \
	$(SRC_DIR)/core/interpreter/import/import.c \
	-lm

	@echo "Build Finished."

# Format all .c and .h files
format:
	@find $(SRC_DIR) -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format-17 -i {} +
	@echo "All source files formatted."

help:
	@echo "Available Builds:"
	@echo "Linux: make build_linux"