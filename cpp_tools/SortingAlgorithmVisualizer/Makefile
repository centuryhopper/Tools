# Project settings
TARGET = SortingAlgorithmsVisualizer
BUILD_DIR = build
WEB_BUILD_DIR = build_web

# Default build type
BUILD_TYPE ?= Release

.PHONY: linux windows wsl web valgrind clean distclean

# ---------------------------------------------------------
# Native Linux / WSL 2 build
linux:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/$(TARGET)

# ---------------------------------------------------------
# Windows native build
windows:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR)
	.\$(BUILD_DIR)\$(TARGET).exe

# ---------------------------------------------------------
# WSL 2 build (from inside WSL terminal)
wsl:
	cd /mnt/c/Users/leoth/Documents/GitHub/Tools/cpp_tools/SortingAlgorithmVisualizer && \
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) && \
	cmake --build $(BUILD_DIR) && \
	./$(BUILD_DIR)/$(TARGET)

# ---------------------------------------------------------
# Web (Emscripten) build
web:
	#source ~/emsdk/emsdk_env.fish &&
	mkdir -p $(WEB_BUILD_DIR) && \
	emcmake cmake -B $(WEB_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release && \
	cmake --build $(WEB_BUILD_DIR)

# ---------------------------------------------------------
# Debug build + Valgrind (Linux only)
valgrind:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		./$(BUILD_DIR)/$(TARGET)

# ---------------------------------------------------------
# Cleanup
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR) $(WEB_BUILD_DIR)

distclean: clean
	@echo "Removing CMake cache files..."
	rm -rf CMakeFiles CMakeCache.txt


