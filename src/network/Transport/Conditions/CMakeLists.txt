add_library(rtype_network_conditions STATIC
  NetworkConditionsText.cpp
  SimulatorLaunchOptions.cpp
)

target_include_directories(rtype_network_conditions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_conditions
  PUBLIC
    rtype_network_exceptions
  PRIVATE
    rtype_network_transport
)

rtype_enable_warnings(rtype_network_conditions)
