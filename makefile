PROJECT_NAME := otu4_datatype_test
VERSION := 1.0.0

# -----------------------------------------------------------------------------
# Пути и директории
# -----------------------------------------------------------------------------
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep

# -----------------------------------------------------------------------------
# Исходные файлы
# -----------------------------------------------------------------------------
# Находим все .cpp файлы
MAIN_SRC := main.cpp
SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)

# Все исходные файлы
ALL_SRCS := $(MAIN_SRC) $(SRC_CPP)

# Объектные файлы для src/ файлов
SRC_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/src/%.o, $(SRC_CPP))
# Объектный файл для main.cpp
MAIN_OBJ := $(OBJ_DIR)/main.o

# Все объектные файлы
OBJS := $(MAIN_OBJ) $(SRC_OBJS)

# Файлы зависимостей
DEPS := $(patsubst $(OBJ_DIR)/%.o, $(DEP_DIR)/%.d, $(OBJS))

# -----------------------------------------------------------------------------
# Файлы заголовков
# -----------------------------------------------------------------------------
INC_FILES := $(wildcard $(INC_DIR)/*.hpp)
INC_FLAGS := $(addprefix -I, $(INC_DIR))

# -----------------------------------------------------------------------------
# Настройки компилятора
# -----------------------------------------------------------------------------
CXX := g++
CXX_STANDARD := c++17
CXX_FLAGS := -std=$(CXX_STANDARD) -Wall -Wextra -pedantic -O2 -g3
CXX_FLAGS += $(INC_FLAGS)
CXX_FLAGS += -MMD -MP

# Для отладочной версии
DEBUG_FLAGS := -DDEBUG -DVERBOSE -g -O0
RELEASE_FLAGS := -DNDEBUG -O3

# -----------------------------------------------------------------------------
# Цели сборки
# -----------------------------------------------------------------------------
.PHONY: all debug release rebuild rebuild-debug rebuild-release \
        clean clean-all help test run info setup

# По умолчанию собираем debug версию
all: debug

# Debug версия
debug: CXX_FLAGS += $(DEBUG_FLAGS)
debug: $(BIN_DIR)/$(PROJECT_NAME)_debug

# Release версия
release: CXX_FLAGS += $(RELEASE_FLAGS)
release: $(BIN_DIR)/$(PROJECT_NAME)

# -----------------------------------------------------------------------------
# Пересборка
# -----------------------------------------------------------------------------

# Пересборка debug версии
rebuild-debug:
	@echo "Пересборка debug версии..."
	@$(MAKE) clean
	@$(MAKE) debug

# Пересборка release версии  
rebuild-release:
	@echo "Пересборка release версии..."
	@$(MAKE) clean
	@$(MAKE) release

# Пересборка текущей версии (по умолчанию debug)
rebuild: rebuild-debug

# Быстрая пересборка без изменения конфигурации
quick-rebuild: clean
	@echo "Быстрая пересборка..."
	@$(MAKE) debug

# -----------------------------------------------------------------------------
# Правила сборки
# -----------------------------------------------------------------------------

# Создание директорий
$(OBJ_DIR)/src $(DEP_DIR)/src $(BIN_DIR):
	@mkdir -p $@

# Компиляция .cpp файлов из src/
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/src $(DEP_DIR)/src
	@echo "[CC] $< -> $@"
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/src/$(basename $(notdir $@)).d)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

# Компиляция main.cpp
$(OBJ_DIR)/main.o: main.cpp $(INC_FILES) | $(OBJ_DIR)
	@echo "[CC] $< -> $@"
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/main.d)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

# Создание obj и dep директорий для main
$(OBJ_DIR) $(DEP_DIR):
	@mkdir -p $@

# Линковка debug версии
$(BIN_DIR)/$(PROJECT_NAME)_debug: $(OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	@$(CXX) $(OBJS) -o $@

# Линковка release версии
$(BIN_DIR)/$(PROJECT_NAME): $(OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	@$(CXX) $(OBJS) -o $@

# -----------------------------------------------------------------------------
# Утилиты и вспомогательные цели
# -----------------------------------------------------------------------------

# Запуск тестов
run: debug
	@echo "Запуск debug версии..."
	@./$(BIN_DIR)/$(PROJECT_NAME)_debug

run-release: release
	@echo "Запуск release версии..."
	@./$(BIN_DIR)/$(PROJECT_NAME)

# Запуск после пересборки
rerun: rebuild
	@echo "Запуск после пересборки..."
	@./$(BIN_DIR)/$(PROJECT_NAME)_debug

# Запуск с Valgrind (для отладки утечек памяти)
valgrind: debug
	@echo "Запуск с Valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(PROJECT_NAME)_debug

# Запуск с GDB
gdb: debug
	@echo "Запуск с GDB..."
	@gdb ./$(BIN_DIR)/$(PROJECT_NAME)_debug

# Запуск с отладчиком после пересборки
gdb-rebuild: rebuild
	@echo "Запуск с GDB после пересборки..."
	@gdb ./$(BIN_DIR)/$(PROJECT_NAME)_debug

# Форматирование кода
format:
	@echo "Форматирование кода..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i; \
	else \
		echo "clang-format не установлен. Установите: sudo apt-get install clang-format"; \
	fi

# Проверка стиля кода
check-style:
	@echo "Проверка стиля кода..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		find . -name "*.cpp" -o -name "*.hpp" | xargs cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive; \
	else \
		echo "cppcheck не установлен. Установите: sudo apt-get install cppcheck"; \
	fi

# Очистка
clean:
	@echo "Очистка собранных файлов..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

clean-all: clean
	@echo "Полная очистка..."
	@rm -f *.o *.d
	@rm -rf docs

# Информация о проекте
info:
	@echo "=========================================="
	@echo "Проект: $(PROJECT_NAME) v$(VERSION)"
	@echo "=========================================="
	@echo "Исходные файлы:"
	@echo "  main.cpp"
	@for f in $(SRC_CPP); do echo "  $$f"; done
	@echo "Объектные файлы:"
	@for f in $(OBJS); do echo "  $$f"; done
	@echo "Заголовки:"
	@for f in $(INC_FILES); do echo "  $$f"; done
	@echo ""
	@echo "Пути:"
	@echo "  Исходники: $(SRC_DIR)"
	@echo "  Заголовки: $(INC_DIR)"
	@echo "  Сборка:    $(BUILD_DIR)"
	@echo "  Библиотеки: $(BIN_DIR)"
	@echo "Компилятор: $(CXX) $(CXX_STANDARD)"
	@echo "Флаги: $(CXX_FLAGS)"
	@echo "=========================================="

# Вывод справки
help:
	@echo "Доступные цели:"
	@echo "  Основные цели сборки:"
	@echo "    all             - Сборка debug версии (по умолчанию)"
	@echo "    debug           - Сборка с отладочной информацией"
	@echo "    release         - Сборка оптимизированной версии"
	@echo ""
	@echo "  Пересборка:"
	@echo "    rebuild         - Полная пересборка debug версии"
	@echo "    rebuild-debug   - Пересборка debug версии"
	@echo "    rebuild-release - Пересборка release версии"
	@echo "    quick-rebuild   - Быстрая пересборка без clean"
	@echo ""
	@echo "  Очистка:"
	@echo "    clean           - Удаление собранных файлов"
	@echo "    clean-all       - Полная очистка"
	@echo ""
	@echo "  Запуск и отладка:"
	@echo "    run             - Запуск debug версии"
	@echo "    run-release     - Запуск release версии"
	@echo "    rerun           - Пересборка и запуск"
	@echo "    valgrind        - Запуск с проверкой утечек памяти"
	@echo "    gdb             - Запуск под отладчиком"
	@echo "    gdb-rebuild     - Пересборка и запуск под отладчиком"
	@echo ""
	@echo "  Утилиты:"
	@echo "    format          - Форматирование кода"
	@echo "    check-style     - Проверка стиля кода"
	@echo "    doxygen         - Генерация документации"
	@echo "    info            - Информация о проекте"
	@echo "    help            - Эта справка"
	@echo "    setup           - Настройка структуры проекта"
	@echo ""
	@echo "  Примеры:"
	@echo "    make debug              # Собрать отладочную версию"
	@echo "    make rebuild && make run # Полная пересборка и запуск"
	@echo "    make clean release      # Очистка и сборка release"
	@echo "    make gdb                # Отладка в gdb"

# -----------------------------------------------------------------------------
# Тестирование
# -----------------------------------------------------------------------------
TEST_DIR := tests
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)

# Если есть тесты, добавляем цели
ifneq ($(TEST_SRCS),)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/tests/%.o, $(TEST_SRCS))
TEST_BIN := $(BIN_DIR)/test_$(PROJECT_NAME)

.PHONY: test test-run test-rebuild

# Компиляция тестов
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/tests $(DEP_DIR)/tests
	@echo "[CC-TEST] $< -> $@"
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/tests/$(basename $(notdir $@)).d)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJ_DIR)/tests $(DEP_DIR)/tests:
	@mkdir -p $@

# Сборка тестов
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	@echo "[LD-TEST] $@"
	@$(CXX) $(TEST_OBJS) $(filter-out $(OBJ_DIR)/main.o, $(OBJS)) -o $@

# Пересборка тестов
test-rebuild:
	@echo "Пересборка тестов..."
	@$(MAKE) clean
	@$(MAKE) test

# Запуск тестов
test-run: test
	@echo "Запуск тестов..."
	@./$(TEST_BIN)

# Пересборка и запуск тестов
test-rerun: test-rebuild
	@echo "Запуск тестов после пересборки..."
	@./$(TEST_BIN)

# Добавляем цели в help
help::
	@echo ""
	@echo "  Тестирование:"
	@echo "    test           - Собрать тесты"
	@echo "    test-run       - Собрать и запустить тесты"
	@echo "    test-rebuild   - Пересборка тестов"
	@echo "    test-rerun     - Пересборка и запуск тестов"
endif

# -----------------------------------------------------------------------------
# Включение зависимостей
# -----------------------------------------------------------------------------
-include $(DEPS)

# -----------------------------------------------------------------------------
# Быстрый старт
# -----------------------------------------------------------------------------
setup:
	@echo "Настройка проекта..."
	@mkdir -p $(SRC_DIR) $(INC_DIR) $(TEST_DIR)
	@echo "Директории созданы:"
	@echo "  $(SRC_DIR)/     - исходные файлы .cpp"
	@echo "  $(INC_DIR)/     - заголовочные файлы .hpp"
	@echo "  $(TEST_DIR)/    - тесты (опционально)"
	@echo ""
	@echo "Теперь можно запустить:"
	@echo "  make debug     - собрать отладочную версию"
	@echo "  make run       - собрать и запустить"
	@echo "  make rebuild   - полная пересборка"