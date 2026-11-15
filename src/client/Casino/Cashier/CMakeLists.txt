add_library(rtype_client_casino_cashier STATIC
  CashierAmount.cpp
  CashierLayout.cpp
  CashierPainting.cpp
  CashierRules.cpp
  CashierScreen.cpp
)

target_include_directories(rtype_client_casino_cashier PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_cashier
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_casino_painting
    rtype_client_economy
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_box_corners
    rtype_client_interface_pixel
    rtype_client_key_cap
)

rtype_enable_warnings(rtype_client_casino_cashier)
