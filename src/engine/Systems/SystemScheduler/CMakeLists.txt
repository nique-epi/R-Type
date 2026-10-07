add_library(rtype_engine_system_scheduler STATIC SystemScheduler.cpp)

target_include_directories(rtype_engine_system_scheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_system_scheduler
  PUBLIC
    rtype_engine_systems
    rtype_engine_component_registry
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_system_scheduler)
