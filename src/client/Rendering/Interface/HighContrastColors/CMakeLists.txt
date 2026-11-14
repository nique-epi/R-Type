add_library(rtype_client_high_contrast_colors STATIC HighContrastColors.cpp)

target_include_directories(rtype_client_high_contrast_colors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_high_contrast_colors
  PUBLIC
    rtype_client_interface
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_high_contrast_colors)
