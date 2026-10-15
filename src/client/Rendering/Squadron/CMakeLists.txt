add_library(rtype_client_squadron STATIC Squadron.cpp)

target_include_directories(rtype_client_squadron PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_squadron
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_engine_time
  PRIVATE
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_squadron)
