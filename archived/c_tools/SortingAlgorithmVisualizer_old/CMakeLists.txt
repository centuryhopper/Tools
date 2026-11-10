cmake_minimum_required(VERSION 3.21)  # 3.21+ for CMakePresets.json v3
project(SortingAlgorithmsVisualizer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Sanitizer options (native builds only) ---------------------------------
option(ENABLE_ASAN  "AddressSanitizer: out-of-bounds, use-after-free, leaks" OFF)
option(ENABLE_UBSAN "UndefinedBehaviorSanitizer: overflow, bad shifts, misaligned access" OFF)
option(ENABLE_TSAN  "ThreadSanitizer: data races (cannot combine with ASan)" OFF)

# --- Sources ----------------------------------------------------------------
# CONFIGURE_DEPENDS re-globs when you add/remove .c files, no manual re-run needed.
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# --- raylib from vcpkg ------------------------------------------------------
# vcpkg's raylib target carries its own system libs (GL, X11, winmm, ...),
# so no per-platform link lists are needed anymore.
find_package(raylib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

# --- Web build (Emscripten) -------------------------------------------------
if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WEB)
  target_compile_options(${PROJECT_NAME} PRIVATE -Os)
  target_link_options(${PROJECT_NAME} PRIVATE
    "-sUSE_GLFW=3"
    "-sASYNCIFY"
    "-sINITIAL_MEMORY=67108864"
    "-sFORCE_FILESYSTEM=1"
    "-sEXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"]"
    "-sEXPORTED_RUNTIME_METHODS=ccall"
  )
  if(ENABLE_ASAN OR ENABLE_UBSAN OR ENABLE_TSAN)
    message(WARNING "Sanitizers are ignored for the Emscripten build")
  endif()
  return()
endif()

# --- Sanitizers -------------------------------------------------------------
if(ENABLE_TSAN AND ENABLE_ASAN)
  message(FATAL_ERROR "ThreadSanitizer and AddressSanitizer can't be used together. Pick one.")
endif()

set(SANITIZERS "")
if(ENABLE_ASAN)
  list(APPEND SANITIZERS address)
endif()
if(ENABLE_UBSAN)
  list(APPEND SANITIZERS undefined)
endif()
if(ENABLE_TSAN)
  list(APPEND SANITIZERS thread)
endif()

if(SANITIZERS)
  if(MSVC)
    # MSVC only supports AddressSanitizer.
    if(ENABLE_UBSAN OR ENABLE_TSAN)
      message(WARNING "MSVC supports only ASan; UBSan/TSan ignored")
    endif()
    if(ENABLE_ASAN)
      target_compile_options(${PROJECT_NAME} PRIVATE /fsanitize=address /Zi)
    endif()
  else()
    list(JOIN SANITIZERS "," SANITIZER_LIST)
    message(STATUS "Sanitizers enabled: ${SANITIZER_LIST}")
    target_compile_options(${PROJECT_NAME} PRIVATE
      -fsanitize=${SANITIZER_LIST} -fno-omit-frame-pointer -g)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=${SANITIZER_LIST})
    if(ENABLE_UBSAN)
      # Stop at the first UB report instead of printing and continuing.
      target_compile_options(${PROJECT_NAME} PRIVATE -fno-sanitize-recover=undefined)
    endif()
  endif()
endif()


























































































