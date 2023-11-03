# Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Directories
SRC_DIR = .
LIB_RANDOM_DIR = random
LIB_UTILITIES_DIR = utilities

# Source files
MAIN_SRC = $(SRC_DIR)/main_02_2.cpp
LIB_RANDOM_SRC = $(LIB_RANDOM_DIR)/random.cpp
LIB_UTILITIES_SRC = $(LIB_UTILITIES_DIR)/utilities.cpp

# Object files
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
LIB_RANDOM_OBJ = $(LIB_RANDOM_SRC:.cpp=.o)
LIB_UTILITIES_OBJ = $(LIB_UTILITIES_SRC:.cpp=.o)

# Executable name
EXECUTABLE = my_program

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(MAIN_OBJ) $(LIB_RANDOM_OBJ) $(LIB_UTILITIES_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIB_RANDOM_OBJ): $(LIB_RANDOM_SRC)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIB_UTILITIES_OBJ): $(LIB_UTILITIES_SRC) $(LIB_UTILITIES_DIR)/function.h
	$(CXX) $(CXXFLAGS) -c -o $@ $(LIB_UTILITIES_SRC)

clean:
	rm -f $(EXECUTABLE) $(MAIN_OBJ) $(LIB_RANDOM_OBJ) $(LIB_UTILITIES_OBJ)

.PHONY: clean

