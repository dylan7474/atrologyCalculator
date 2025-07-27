# Makefile for NASA-based Astrology Calculator

# The C compiler to use.
CC = gcc

# The name of the final executable.
TARGET = nasa_astro

# All C source files used in the project.
SRCS = main.c

# CFLAGS: Flags passed to the C compiler.
CFLAGS = -Wall -O2 -std=c99

# LDFLAGS: Flags passed to the linker.
# We need to link the cURL, Jansson, and Math libraries.
LDFLAGS = -lcurl -ljansson -lm

# --- Build Rules ---

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
