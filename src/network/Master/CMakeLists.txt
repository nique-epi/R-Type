find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_network_master STATIC MasterMessageText.cpp)

target_include_directories(rtype_network_master PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_master
  PUBLIC
    rtype_network_exceptions
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_network_constants
    rtype_network_transport
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_network_master)
