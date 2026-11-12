set_project("data_structures")
set_version("1.0.0")
set_xmakever("2.8.5")   -- first version with `xmake test`

set_languages("c++23")
set_warnings("all", "extra", "pedantic")

-- Build modes: xmake f -m debug | release
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
set_defaultmode("debug")

-- Keep compile_commands.json in the project root up to date for clangd.
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- Shared by the app and the tests.
add_includedirs("include")

target("data_structures")
    set_kind("binary")
    add_files("src/*.cpp")
target_end()

-- Tests: the app's sources minus main.cpp, plus the test runner. Run with `xmake test`.
target("tests")
    set_kind("binary")
    set_default(false)   -- built by `xmake test`, not by plain `xmake`
    add_files("src/*.cpp|main.cpp", "tests/test_main.cpp")
    add_tests("unit_tests")
target_end()
