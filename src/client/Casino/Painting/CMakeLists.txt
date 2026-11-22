add_library(rtype_client_casino_painting STATIC
  BulbChase.cpp
  ButlerFrames.cpp
  CasinoBarChip.cpp
  CasinoLighting.cpp
  CasinoPainting.cpp
  ChipSelector.cpp
)

target_include_directories(rtype_client_casino_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_painting
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_language
    rtype_client_pixels
    rtype_client_widgets
    rtype_engine_time
  PRIVATE
    rtype_client_box_corners
    rtype_client_cut_corner_box
    rtype_client_economy
    rtype_client_key_cap
)

rtype_enable_warnings(rtype_client_casino_painting)
