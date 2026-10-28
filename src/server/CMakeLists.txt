add_library(rtype_server_constants INTERFACE)

target_include_directories(rtype_server_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_constants INTERFACE rtype_engine_time)

add_subdirectory(Exceptions)
add_subdirectory(Platform)
add_subdirectory(Application)
add_subdirectory(LaunchOptions)
add_subdirectory(Sessions)
add_subdirectory(Replication)
add_subdirectory(Spawning)
add_subdirectory(Instances)
add_subdirectory(Lobby)
add_subdirectory(Modules)

add_executable(r-type_server main.cpp)

target_link_libraries(r-type_server
  PRIVATE
    rtype_logging
    rtype_server_constants
    rtype_server_modules
    rtype_server_stop_signals
)

rtype_enable_warnings(r-type_server)
