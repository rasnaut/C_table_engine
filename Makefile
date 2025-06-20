CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L.
LDLIBS = -lreadline -lmylib  

SRC_DIR = lib
MAIN_SRC = main.c
LIB_SRC = lib/work_library.c lib/dialogue.c lib/lib_tree_234.c lib/lib_key_array.c lib/graphviz_print.c
LIB_OBJS = $(LIB_SRC:.c=.o)
LIB_STATIC = libmylib.a
TARGET = program

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_STATIC)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(LIB_STATIC): $(LIB_OBJS)
	ar rcs $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) libmylib.a $(SRC_DIR)/*.o

build_debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-Wall -Wextra -std=c11 -g -O0 -I./lib"

valgrind: build_debug
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET)
