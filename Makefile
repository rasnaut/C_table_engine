CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L.
LDLIBS = -lreadline -lmylib  

SRC_DIR = lib
MAIN_SRC = main.c
TARGET = program

all: $(TARGET)

$(TARGET): $(MAIN_SRC) libmylib.a library.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

libmylib.a: $(SRC_DIR)/work_library.o $(SRC_DIR)/library.o $(SRC_DIR)/dialogue.o $(SRC_DIR)/key.o
	ar rcs $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) libmylib.a $(SRC_DIR)/*.o
