if(NOT RTYPE_MODULE_PLUGINS)
  return()
endif()

add_library(rtype_engine_plugin_shelf STATIC PluginShelf.cpp)

target_include_directories(rtype_engine_plugin_shelf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_plugin_shelf
  PUBLIC
    rtype_engine_modules
    rtype_engine_commands
    rtype_engine_shared_library
    rtype_logging_path_text
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_plugin_shelf)
