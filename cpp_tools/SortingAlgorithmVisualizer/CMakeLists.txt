cmake_minimum_required(VERSION 3.15)
project(SortingAlgorithmsVisualizer C)

# --- Project Structure ---
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

# --- Your Local Raylib Path ---
set(RAYLIB_SRC "${CMAKE_SOURCE_DIR}/raylib/src") # Change if needed on Windows
set(RAYLIB_WEB_LIB "${RAYLIB_SRC}/web/libraylib.web.a")

# --- Native Build (Linux/Windows/macOS) ---
if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${RAYLIB_SRC})
    target_link_libraries(${PROJECT_NAME} raylib m dl pthread GL rt X11)

# --- Web Build (Emscripten) ---
else()
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${RAYLIB_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -std=c99
        -Os
        -DPLATFORM_WEB
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sTOTAL_MEMORY=67108864"
        "-sFORCE_FILESYSTEM=1"
        "-sEXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"]"
        "-sEXPORTED_RUNTIME_METHODS=ccall"
    )

     # Linker flags for emcc go here:
    target_link_options(${PROJECT_NAME} PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sTOTAL_MEMORY=67108864"
        "-sFORCE_FILESYSTEM=1"
        "-sEXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"]"
        "-sEXPORTED_RUNTIME_METHODS=ccall"
    )

    target_link_libraries(${PROJECT_NAME} ${RAYLIB_WEB_LIB})
endif()

# --- Valgrind Target (Linux only) ---
if (UNIX AND NOT APPLE AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_custom_target(valgrind
        COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
                $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        COMMENT "Running Valgrind memory check..."
    )
endif()
