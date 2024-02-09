# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address

# Project name
EXECUTABLE = webserv

# Directories
SRC_DIRS = src src/config
OBJ_DIR = obj
INCLUDE_DIR = include

# Finds all .cpp files in SRC_DIRS
SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Object files to be created
OBJECTS = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst src/config/%.cpp,$(OBJ_DIR)/%.o,$(OBJECTS))

# Dependencies
INCLUDES = -I$(INCLUDE_DIR)

# The first rule is the one executed when no parameters are fed to the Makefile
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ 

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: src/config/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# To remove generated files
clean:
	rm -f $(OBJ_DIR)/*.o

fclean: clean
	rm -f $(EXECUTABLE)

re: fclean all
