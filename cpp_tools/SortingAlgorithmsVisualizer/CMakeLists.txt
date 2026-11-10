cmake_minimum_required(VERSION 3.21)  # 3.21+ for CMakePresets.json v3
project(SortingAlgorithmsVisualizer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS  "Build unit tests (needs the vcpkg 'tests' feature)" OFF)
option(ENABLE_ASAN  "AddressSanitizer: out-of-bounds, use-after-free, leaks" OFF)
option(ENABLE_UBSAN "UndefinedBehaviorSanitizer: overflow, bad shifts, misaligned access" OFF)
option(ENABLE_TSAN  "ThreadSanitizer: data races (cannot combine with ASan)" OFF)

# --- Third-party packages (all from vcpkg) ----------------------------------
find_package(raylib CONFIG REQUIRED)
find_path(RAYGUI_INCLUDE_DIR raygui.h REQUIRED)   # raygui is header-only

# --- Shared settings: warnings + sanitizers ---------------------------------
if(ENABLE_TSAN AND ENABLE_ASAN)
  message(FATAL_ERROR "ThreadSanitizer and AddressSanitizer can't be used together. Pick one.")
endif()

set(SANITIZERS "")
if(NOT EMSCRIPTEN)
  if(ENABLE_ASAN)
    list(APPEND SANITIZERS address)
  endif()
  if(ENABLE_UBSAN)
    list(APPEND SANITIZERS undefined)
  endif()
  if(ENABLE_TSAN)
    list(APPEND SANITIZERS thread)
  endif()
elseif(ENABLE_ASAN OR ENABLE_UBSAN OR ENABLE_TSAN)
  message(WARNING "Sanitizers are ignored for the Emscripten build")
endif()
if(SANITIZERS)
  list(JOIN SANITIZERS "," SANITIZER_LIST)
  message(STATUS "Sanitizers enabled: ${SANITIZER_LIST}")
endif()

function(apply_project_settings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
    if(ENABLE_ASAN)
      target_compile_options(${target} PRIVATE /fsanitize=address /Zi)
    endif()
    if(ENABLE_UBSAN OR ENABLE_TSAN)
      message(WARNING "MSVC supports only ASan; UBSan/TSan ignored")
    endif()
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra)
    if(SANITIZERS)
      target_compile_options(${target} PRIVATE
        -fsanitize=${SANITIZER_LIST} -fno-omit-frame-pointer -g)
      target_link_options(${target} PRIVATE -fsanitize=${SANITIZER_LIST})
      if(ENABLE_UBSAN)
        # Stop at the first UB report instead of printing and continuing.
        target_compile_options(${target} PRIVATE -fno-sanitize-recover=undefined)
      endif()
    endif()
  endif()
endfunction()

# --- App --------------------------------------------------------------------
# CONFIGURE_DEPENDS re-globs when you add/remove .c files.
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
  "${RAYGUI_INCLUDE_DIR}"
)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
apply_project_settings(${PROJECT_NAME})

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
endif()

# --- Tests ------------------------------------------------------------------
# Each tests/src/test_*.c becomes its own executable, registered with CTest.
if(BUILD_TESTS AND NOT EMSCRIPTEN)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        v2.6.0
  GIT_SHALLOW    TRUE
)
  FetchContent_MakeAvailable(unity)

  file(GLOB TEST_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/src/test_*.c")
  foreach(test_src ${TEST_FILES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_link_libraries(${test_name} PRIVATE unity)
    apply_project_settings(${test_name})
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
