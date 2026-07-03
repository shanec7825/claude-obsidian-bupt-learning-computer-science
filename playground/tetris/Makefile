# Tetris Game (educ8s/raylib) — Simplified Makefile for learning
# Original complex raylib Makefile preserved at .raw/Cpp-Tetris-Game-with-raylib/Makefile

.PHONY: all clean run

PROJECT_NAME = tetris
SRC_DIR      = src
OBJ_DIR      = build
TARGET       = $(PROJECT_NAME).exe

# --- Compiler & Flags ---
CXX      = g++
CXXFLAGS = -Wall -std=c++17 -O2
INCLUDE  = -I$(SRC_DIR)

# --- Source Files ---
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# --- raylib (Windows, MinGW-w64) ---
RAYLIB_PATH = C:/raylib
ifeq ($(OS),Windows_NT)
	RAYLIB_INC = -I$(RAYLIB_PATH)/include
	RAYLIB_LIB = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
endif

all: $(TARGET)

$(TARGET): $(OBJ_DIR) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) $(RAYLIB_LIB)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(RAYLIB_INC) $(INCLUDE)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

