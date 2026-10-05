add_library(rtype_engine_entity_registry STATIC EntityRegistry.cpp)

target_include_directories(rtype_engine_entity_registry
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

rtype_enable_warnings(rtype_engine_entity_registry)
