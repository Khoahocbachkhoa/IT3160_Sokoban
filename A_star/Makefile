CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
TARGET = sokoban_solver

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/board.cpp $(SRC_DIR)/search.cpp $(SRC_DIR)/deadlock.cpp $(SRC_DIR)/distance.cpp $(SRC_DIR)/heuristic.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

run: $(TARGET)
	./$(TARGET) < ../data/level1.txt

clean:
	rm -f $(OBJECTS) $(TARGET)

test: $(TARGET)
	@echo "Testing level1..."
	./$(TARGET) < ../data/level1.txt
	@echo "\nTesting level2..."
	./$(TARGET) < ../data/level2.txt
