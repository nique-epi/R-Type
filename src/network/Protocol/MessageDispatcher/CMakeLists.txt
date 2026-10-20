add_library(rtype_network_message_dispatcher STATIC MessageDispatcher.cpp)

target_include_directories(rtype_network_message_dispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_message_dispatcher
  PUBLIC
    rtype_network_protocol
    rtype_network_exceptions
    rtype_network_constants
    rtype_logging
)

rtype_enable_warnings(rtype_network_message_dispatcher)
