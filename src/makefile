.PHONY: all debug clean build build-dir bin-dir

CC = gcc
CFLAGS = -Wall

C_OBJECTS = preprocessor.o string_utils.o symbol_table.o
EXEC = preprocessor

INCLUDE = ../include
LIB = ../lib
BUILD = ../build
BIN = ../bin

CLI_ARGS = 2> error

all: build-dir bin-dir build
	@echo Running ... $(EXEC)
	@$(BIN)/$(EXEC) $(CLI_ARGS)

build: $(C_OBJECTS)
	@echo [+] Linking ...
	@cd $(BUILD) && $(CC) -o $(BIN)/$(EXEC) $(C_OBJECTS) $(CFLAGS) $(LIBS)

$(C_OBJECTS): %.o: %.c
	@echo [+] Compiling ... $<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(BUILD)/$@

debug: CFLAGS += -DDEBUG
debug: CLI_ARGS = 
debug: all

build-dir:
	@mkdir -p $(BUILD)
bin-dir:
	@mkdir -p $(BIN)

clean:
	@echo [-] Cleaning ...
	@rm $(BUILD)/*
