add_subdirectory(Exceptions)
add_subdirectory(Time)
add_subdirectory(EntityRegistry)

add_library(rtype_engine INTERFACE)

target_include_directories(rtype_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine
  INTERFACE
    rtype_engine_exceptions
    rtype_engine_time
    rtype_engine_system_clock
    rtype_engine_fixed_timestep
    rtype_engine_timer_scheduler
    rtype_engine_entity_registry
)
