set_project("similar_files_checker")
set_version("1.0.0")
set_xmakever("2.8.5") -- first version with `xmake test`

set_languages("c++26", "c23")
set_toolchains("gcc-16") -- g++-16; change or remove to use another compiler
set_warnings("all", "extra", "pedantic")

-- Build modes: xmake f -m debug | release
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
set_defaultmode("release")

-- Keep compile_commands.json in the project root up to date for clangd.
add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

-- Sanitizer, combinable with either mode:
--   xmake f -m debug --sanitizer=address
--   xmake f -m release --sanitizer=thread
-- Only this project's code is instrumented, not the packages.
option("sanitizer")
set_default("none")
set_showmenu(true)
set_values("none", "address", "thread")
set_description("Sanitizer to build with: none, address or thread")
option_end()

local sanitizer = get_config("sanitizer")
if sanitizer == "address" then
	set_policy("build.sanitizer.address", true)
	add_cxflags("-fno-omit-frame-pointer")
elseif sanitizer == "thread" then
	set_policy("build.sanitizer.thread", true)
	add_cxflags("-fno-omit-frame-pointer")
end

-- Bounds checking in debug builds. libstdc++ reads the first macro,
-- libc++ the second; each ignores the other.
if is_mode("debug") then
	add_defines("_GLIBCXX_ASSERTIONS", "_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST")
end

-- exiv2 isn't in xmake's package repository, so this builds it from source
-- with its own CMake build. Optional features we don't use are switched off
-- to avoid extra dependencies.
package("exiv2")
set_homepage("https://exiv2.org")
set_license("GPL-2.0")
add_urls("https://github.com/Exiv2/exiv2.git")
add_versions("0.28.9", "v0.28.9") -- version -> git tag
add_deps("cmake")
add_deps("expat", "zlib") -- XMP metadata and PNG support
add_links("exiv2", "exiv2-xmp")
on_install("linux", "macosx", function(package)
	local configs = {
		"-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"),
		"-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"),
		"-DEXIV2_ENABLE_XMP=ON",
		"-DEXIV2_ENABLE_PNG=ON",
		"-DEXIV2_ENABLE_BMFF=ON",
		"-DEXIV2_ENABLE_INIH=OFF",
		"-DEXIV2_ENABLE_BROTLI=OFF",
		"-DEXIV2_ENABLE_NLS=OFF",
		"-DEXIV2_ENABLE_CURL=OFF",
		"-DEXIV2_ENABLE_WEBREADY=OFF",
		"-DEXIV2_BUILD_SAMPLES=OFF",
		"-DEXIV2_BUILD_EXIV2_COMMAND=OFF",
		"-DEXIV2_BUILD_UNIT_TESTS=OFF",
		"-DEXIV2_BUILD_FUZZ_TESTS=OFF",
		"-DEXIV2_BUILD_DOC=OFF",
	}
	import("package.tools.cmake").install(package, configs)
end)
package_end()

-- Third-party packages, downloaded and built by xmake on the first build.
-- OpenCV and FFmpeg take a long time the first time; later builds reuse them.
add_requires("fmt", "argparse", "blake3", "tbb", "ffmpeg", "xxhash")
add_requires("exiv2 0.28.9")
add_requires("opencv", { configs = { ffmpeg = true } }) -- video support via FFmpeg

-- Settings shared by the app and the tests.
add_includedirs("include")
add_packages("fmt", "exiv2", "argparse", "blake3", "tbb", "ffmpeg", "xxhash", "opencv")

target("similar_files_checker")
set_kind("binary")
add_files("src/*.cpp", "src/*.c")
set_rundir("$(projectdir)") -- `xmake run` starts in the project root, so relative paths work
target_end()

-- Tests: the app's sources minus main.cpp, plus the test runner. Run with `xmake test`.
target("tests")
set_kind("binary")
set_default(false) -- built by `xmake test`, not by plain `xmake`
add_files("src/*.cpp|main.cpp", "src/*.c", "tests/test_main.cpp")
add_defines('TEST_DATA_DIR="' .. path.join(os.projectdir(), "tests", "directory_contents") .. '"')
add_tests("unit_tests")
target_end()
