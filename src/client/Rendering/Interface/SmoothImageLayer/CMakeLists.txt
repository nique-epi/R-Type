add_library(rtype_client_smooth_image_layer STATIC SmoothImageLayer.cpp)

target_include_directories(rtype_client_smooth_image_layer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_smooth_image_layer
  PUBLIC
    rtype_client_interface
    rtype_logging
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_smooth_image_layer)
