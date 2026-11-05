find_package(httplib CONFIG REQUIRED)

add_library(rtype_network_master_client STATIC MasterClient.cpp)

target_include_directories(rtype_network_master_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_master_client
  PUBLIC
    rtype_network_master_messages
    rtype_network_master_routes
  PRIVATE
    httplib::httplib
    rtype_network_exceptions
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_network_master_client)
