# название файла с точкой входа
EXE := main.out

# директории, в которых должны храниться исходники, заголовочные файлы + объектные файлы
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj


# cpp файлы для структуры InOutData320 + main.cpp
SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)
MAIN_CPP := main.cpp
ALL_SRCS := $(SRC_CPP) $(MAIN_CPP)


SRC_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_CPP))
MAIN_OBJ := $(OBJ_DIR)/main.o

# отладочная информация
$(info $$SRC_OBJS = $(SRC_OBJS))

# переменные для компиляции
CXX := g++
CXX_STANDARD := c++17
CXX_FLAGS := -std=$(CXX_STANDARD) -Wall -Wextra -pedantic -O2 -g3

.PHONY: clean all run

run: $(EXE)
	@./$(EXE)

# .PHONY инструкция для удаления всех созданных объектных и исполняемых файлов
clean:
	@rm -rf $(OBJ_DIR)
	@if [ -f "$(EXE)" ]; then rm "$(EXE)" ; fi

all: $(EXE)

# создание директории obj
$(OBJ_DIR):
	@mkdir -p $@

# создание исполняемого файла
$(EXE): $(MAIN_OBJ) $(SRC_OBJS)
	$(CXX) $^ -o $@

# создание объектных файлов на основе содержимого директории $(SRC_DIR)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

# создание объектного файла main.o на основе содержимого $(MAIN_CPP)
$(MAIN_OBJ): $(MAIN_CPP) | $(OBJ_DIR)
	$(CXX) $(CXX_FLAGS) -c $< -o $@