# Makefile for People Graph CLI

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -O2 -MMD -MP -I.
LDFLAGS  :=
LDLIBS   := -lreadline

OBJDIR   := build
LIBNAME  := libpeoplegraph.a
TARGET   := program
LIBDIR   := lib/graph

# library sources (adjust paths if needed)
LIB_SRC := \
  $(LIBDIR)/lib_vertex_handle.c \
  $(LIBDIR)/lib_edge.c \
  $(LIBDIR)/lib_edge_sorted_cycle_list.c \
  $(LIBDIR)/lib_vertex.c \
  $(LIBDIR)/lib_graph.c \
  lib/graphviz_print.c \
  lib/dialogue.c

LIB_OBJS := $(LIB_SRC:%.c=$(OBJDIR)/%.o)
LIB_DEPS := $(LIB_OBJS:.o=.d)

MAIN_SRC := main.c

.PHONY: all clean run build_debug asan valgrind

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIBNAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC) $(LIBNAME) $(LDLIBS)

$(LIBNAME): $(LIB_OBJS)
	ar rcs $@ $^

# Pattern rule: build objects into $(OBJDIR) mirroring source tree
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(LIBNAME) $(TARGET)

build_debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-Wall -Wextra -std=c11 -g -O0 -MMD -MP -I." all

asan:
	$(MAKE) clean
	$(MAKE) CFLAGS="-Wall -Wextra -std=c11 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -MMD -MP -I." \
	        LDFLAGS="-fsanitize=address,undefined" all

valgrind: build_debug
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET)

# auto-deps for library objects
-include $(LIB_DEPS)
