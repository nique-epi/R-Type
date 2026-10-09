add_library(rtype_engine_events INTERFACE)

target_include_directories(rtype_engine_events INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(EventBus)
