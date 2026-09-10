CC      := gcc
CFLAGS  := -Wall -Wextra -g
SRC_DIR := src
SOURCES := $(SRC_DIR)/main.c $(SRC_DIR)/graph.c $(SRC_DIR)/io.c $(SRC_DIR)/algorithms.c
TARGET  := main.exe

# Run from the project root -- the program reads/writes data/ with
# paths relative to the current directory (e.g. data/facebook_named.txt).

.PHONY: all run data clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(SRC_DIR)/graph.h $(SRC_DIR)/io.h $(SRC_DIR)/algorithms.h $(SRC_DIR)/colors.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Regenerate the decompressed SNAP edge list and the named network from it.
data: data/facebook_combined.txt data/facebook_named.txt

data/facebook_combined.txt: data/facebook_combined.txt.gz
	gunzip -k data/facebook_combined.txt.gz

data/facebook_named.txt: data/facebook_combined.txt.gz tools/convert_snap.py
	python tools/convert_snap.py

clean:
	rm -f $(TARGET) *.o
