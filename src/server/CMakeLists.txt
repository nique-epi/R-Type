add_library(rtype_server_constants INTERFACE)

target_include_directories(rtype_server_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_constants INTERFACE rtype_engine_time)

add_subdirectory(Platform)
add_subdirectory(Application)

add_executable(r-type_server main.cpp)

target_link_libraries(r-type_server
  PRIVATE
    rtype_game
    rtype_logging
    rtype_network_constants
    rtype_server_application
    rtype_server_constants
)

rtype_enable_warnings(r-type_server)
