add_library(rtype_server_rtype_systems STATIC RtypeSystems.cpp)

target_include_directories(rtype_server_rtype_systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rtype_systems
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_systems
    rtype_game
)

rtype_enable_warnings(rtype_server_rtype_systems)
