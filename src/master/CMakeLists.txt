add_library(rtype_master_constants INTERFACE)

target_include_directories(rtype_master_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Exceptions)
add_subdirectory(Http)
