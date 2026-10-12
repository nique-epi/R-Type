add_subdirectory(Exceptions)
add_subdirectory(Time)
add_subdirectory(EntityRegistry)
add_subdirectory(Components)
add_subdirectory(Systems)
add_subdirectory(Events)
add_subdirectory(Collision)

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
    rtype_engine_components
    rtype_engine_entity_index_map
    rtype_engine_component_registry
    rtype_engine_systems
    rtype_engine_system_scheduler
    rtype_engine_events
    rtype_engine_event_bus
    rtype_engine_collision
)
