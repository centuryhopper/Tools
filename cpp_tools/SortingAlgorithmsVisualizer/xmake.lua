set_project("SortingAlgorithmsVisualizer")
set_version("0.1.0")
set_xmakever("2.8.5") -- first version with `xmake test`

set_languages("gnu23")
set_warnings("all", "extra")

-- Build modes: xmake f -m <mode>
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release", "asan", "tsan")
set_defaultmode("debug")

-- Keep compile_commands.json in the project root up to date for clangd.
add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

-- Sanitizer modes. Only our own code is instrumented, not the packages.
-- ASan and TSan can't be combined, so they are separate modes.
if is_mode("asan") then
	set_symbols("debug")
	set_optimize("none")
	set_policy("build.sanitizer.address", true)
	set_policy("build.sanitizer.undefined", true)
	-- Stop at the first UB report instead of printing and continuing.
	add_cflags("-fno-omit-frame-pointer", "-fno-sanitize-recover=undefined")
elseif is_mode("tsan") then
	set_symbols("debug")
	set_optimize("none")
	set_policy("build.sanitizer.thread", true)
	add_cflags("-fno-omit-frame-pointer")
end

-- Hides GPU driver leaks from LeakSanitizer (see lsan.supp).
local lsan_options = "suppressions=" .. path.join(os.projectdir(), "lsan.supp") .. ":print_suppressions=0"

-- raygui is a single header. xmake's own raygui package doesn't support the
-- web (wasm) platform, so this small package fetches the header itself and
-- works on every platform.
package("raygui_h")
set_kind("library", { headeronly = true })
set_homepage("https://github.com/raysan5/raygui")
set_license("zlib")
add_urls("https://github.com/raysan5/raygui.git")
add_versions("4.0", "4.0") -- version -> git tag
on_install(function(package)
	os.cp("src/raygui.h", package:installdir("include"))
end)
package_end()

-- Third-party packages, downloaded and built by xmake on the first build.
add_requires("raylib", { system = false })
add_requires("raygui_h 4.0")

target("SortingAlgorithmsVisualizer")
set_kind("binary")
add_files("src/*.c")
add_includedirs("include")
add_packages("raylib", "raygui_h")
set_runenv("LSAN_OPTIONS", lsan_options)

-- Web build: xmake f -p wasm -m release
if is_plat("wasm") then
	set_extension(".html")
	set_optimize("smallest")
	add_defines("PLATFORM_WEB")
	add_ldflags(
		"-sUSE_GLFW=3",
		"-sASYNCIFY",
		"-sINITIAL_MEMORY=67108864",
		"-sFORCE_FILESYSTEM=1",
		"-sEXPORTED_FUNCTIONS=_free,_malloc,_main",
		"-sEXPORTED_RUNTIME_METHODS=ccall",
		{ force = true }
	)
end
target_end()

-- Tests: every tests/src/test_*.c becomes its own executable, run by `xmake test`.
if not is_plat("wasm") then
	add_requires("unity_test", { system = false })

	for _, file in ipairs(os.files("tests/src/test_*.c")) do
		target(path.basename(file))
		set_kind("binary")
		set_default(false) -- built by `xmake test`, not by plain `xmake`
		set_group("tests")
		add_files(file)
		add_includedirs("include")
		add_packages("unity_test")
		add_tests("default", { runenvs = { LSAN_OPTIONS = lsan_options } })
		target_end()
	end
end
