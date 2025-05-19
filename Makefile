
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L./lib
LDLIBS = -lreadline -lmylib  


SRC_DIR = lib
MAIN_SRC = main.c
TARGET = program


all: $(TARGET)

$(TARGET): $(MAIN_SRC) libmylib.a
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

libmylib.a: $(SRC_DIR)/work_library.o $(SRC_DIR)/library.o $(SRC_DIR)/dialogue.o
	ar rcs $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
