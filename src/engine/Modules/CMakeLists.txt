add_subdirectory(Stages)
add_subdirectory(Services)
add_subdirectory(InstallContext)

add_library(rtype_engine_modules INTERFACE)

target_include_directories(rtype_engine_modules INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_modules INTERFACE rtype_engine_exceptions rtype_engine_install_context)

# The export list of a plugin on Windows: rtype_add_plugin adds it to the sources of each plugin.
set(RTYPE_PLUGIN_ENTRY_DEFINITION ${CMAKE_CURRENT_SOURCE_DIR}/PluginEntry.def CACHE INTERNAL "The module-definition file that exports rtypePluginEntry")

add_subdirectory(ModuleSwitches)
add_subdirectory(PluginShelf)
add_subdirectory(Application)
