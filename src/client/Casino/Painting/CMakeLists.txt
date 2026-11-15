add_library(rtype_client_casino_painting STATIC
  CasinoBarChip.cpp
  CasinoPainting.cpp
)

target_include_directories(rtype_client_casino_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_painting
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_economy
)

rtype_enable_warnings(rtype_client_casino_painting)
