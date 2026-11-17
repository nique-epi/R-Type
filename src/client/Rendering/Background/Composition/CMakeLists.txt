add_library(rtype_client_background_composition STATIC LayerCanvas.cpp)

target_include_directories(rtype_client_background_composition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background_composition
  PUBLIC
    SFML::Graphics
    rtype_client_background
    rtype_client_background_layers
    rtype_client_pixel_batch
)

rtype_enable_warnings(rtype_client_background_composition)
