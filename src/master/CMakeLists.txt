add_library(rtype_master_constants INTERFACE)

target_include_directories(rtype_master_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Exceptions)
add_subdirectory(LaunchOptions)
add_subdirectory(Logger)
add_subdirectory(Storage)
add_subdirectory(Http)
add_subdirectory(ServerKeys)
add_subdirectory(Modules)
