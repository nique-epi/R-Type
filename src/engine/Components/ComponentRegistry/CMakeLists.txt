add_library(rtype_engine_component_registry STATIC ComponentRegistry.cpp)

target_include_directories(rtype_engine_component_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_component_registry
  PUBLIC
    rtype_engine_components
    rtype_engine_entity_registry
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_component_registry)
