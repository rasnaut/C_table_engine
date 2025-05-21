CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L.
LDLIBS = -lreadline -lmylib  

SRC_DIR = lib
MAIN_SRC = main.c
LIB_SRC = lib/library.c lib/work_library.c lib/dialogue.c lib/list.c
LIB_OBJS = $(LIB_SRC:.c=.o)
LIB_STATIC = libmylib.a
TARGET = program

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_STATIC)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

libmylib.a: $(SRC_DIR)/work_library.o $(SRC_DIR)/library.o $(SRC_DIR)/dialogue.o $(SRC_DIR)/list.o
	ar rcs $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) libmylib.a $(SRC_DIR)/*.o
