add_library(rtype_engine_event_bus STATIC EventBus.cpp)

target_include_directories(rtype_engine_event_bus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_event_bus
  PUBLIC
    rtype_engine_events
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_event_bus)
