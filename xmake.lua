---@diagnostic disable: undefined-global, codestyle-check, undefined-field
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = ".", lsp = "clangd" })

target("base")
    set_kind("static")
    set_default(false)
    set_languages("cxx23", { public = true })
    add_forceincludes("src/common.hh", { public = true, force = true })
    set_pcheader("src/common.hh", { public = true, force = true })
    add_includedirs("src/", { public = true })
    add_files(
        "src/file.cc",
        "src/os.cc",
        "src/stack.cc"
    )
target_end()

for _, file in ipairs(os.files("src/sol/day*.cc")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        add_includedirs("src/", { public = true })
        add_files("src/sol/" .. name .. ".cc")
        add_deps("base")
    target_end()
end
