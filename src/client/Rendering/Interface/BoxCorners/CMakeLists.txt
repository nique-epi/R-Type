add_library(rtype_client_box_corners STATIC BoxCorners.cpp)

target_include_directories(rtype_client_box_corners PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_box_corners
  PUBLIC
    rtype_client_interface
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_box_corners)
