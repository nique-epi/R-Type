add_library(rtype_client_easing STATIC Easing.cpp)

target_include_directories(rtype_client_easing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_easing
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_easing)
