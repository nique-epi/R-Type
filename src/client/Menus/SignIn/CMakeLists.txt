add_library(rtype_client_menus_sign_in STATIC
  SignInScreen.cpp
  SignInText.cpp
  WaitingCodePainting.cpp
)

target_include_directories(rtype_client_menus_sign_in PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_sign_in
  PUBLIC
    SFML::Graphics
    rtype_client_account_session
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_connection_device_link
    rtype_client_interface
    rtype_client_menu
    rtype_client_screen_registry
  PRIVATE
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_menus_sign_in)
