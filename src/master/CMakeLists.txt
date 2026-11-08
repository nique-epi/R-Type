add_library(rtype_master_constants INTERFACE)

configure_file(MasterVersionConstants.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/MasterVersionConstants.hpp @ONLY)

target_include_directories(rtype_master_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Exceptions)
add_subdirectory(LaunchOptions)
add_subdirectory(Logger)
add_subdirectory(Storage)
add_subdirectory(Http)
add_subdirectory(Probe)
add_subdirectory(Servers)
add_subdirectory(Leaderboard)
add_subdirectory(Modules)
