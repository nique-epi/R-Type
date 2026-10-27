add_library(rtype_engine_modules INTERFACE)

target_include_directories(rtype_engine_modules INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Application)
