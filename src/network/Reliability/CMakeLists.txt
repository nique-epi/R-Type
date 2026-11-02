add_library(rtype_network_reliability STATIC
  GuaranteedDelivery.cpp
  ReceivedSequences.cpp
  UnacknowledgedMessages.cpp
)

target_include_directories(rtype_network_reliability PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_reliability
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_exceptions
    rtype_network_sequence_order
)

rtype_enable_warnings(rtype_network_reliability)
