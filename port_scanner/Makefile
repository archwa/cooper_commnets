.PHONY:  clean run

# Define build vars
SRC_DIR=src
INC_DIR=inc
TARGET_BIN=scanner
BUILT_FILES=*.o $(TARGET_BIN)


# Build target binary
$(TARGET_BIN): $(SRC_DIR)/*.c
	@echo "Building '$@' ..."
	gcc -I$(INC_DIR) -o$@ $^ 

# Run target binary
run: $(TARGET_BIN)
	./$(TARGET_BIN) ${ARGS}

# Clean all built files
clean:
	@echo "Cleaning all built files ..."
	rm -f $(BUILT_FILES)
