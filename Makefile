# Makefile for miniShell project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude

# Source and object directories
SRCDIR = src
OBJDIR = bin

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)

# Object files (derived from source files)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Output executable
TARGET = my_shell

# Default rule to build the executable
all: $(TARGET)

# Rule to build the executable from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Rule to build object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

# Phony targets (not real files)
.PHONY: all clean

