add_library(rtype_network_protocol INTERFACE)

target_include_directories(rtype_network_protocol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_protocol
  INTERFACE
    rtype_identifiers
    rtype_network_serialization
    rtype_network_transport
)

add_library(rtype_network_wire_types STATIC WireTypes.cpp)

target_link_libraries(rtype_network_wire_types
  PUBLIC
    rtype_network_protocol
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_wire_types)

add_subdirectory(WorldStateCodec)
add_subdirectory(DatagramCodec)
add_subdirectory(Messages)
add_subdirectory(MessageDispatcher)
