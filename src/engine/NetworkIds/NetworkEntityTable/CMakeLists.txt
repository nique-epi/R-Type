add_library(rtype_engine_network_entity_table STATIC NetworkEntityTable.cpp)

target_include_directories(rtype_engine_network_entity_table PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_network_entity_table
  PUBLIC
    rtype_engine_entity_registry
    rtype_engine_network_id_constants
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_network_entity_table)
