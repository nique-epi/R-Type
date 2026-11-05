add_library(rtype_client_connection_entity_types STATIC SessionEntityTypes.cpp)

target_include_directories(rtype_client_connection_entity_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_entity_types
  PUBLIC
    rtype_network_connection_accepted
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_connection_entity_types)
