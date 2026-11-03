add_library(rtype_client_input STATIC
  ActionSet.cpp
  KeyBindings.cpp
  KeyNames.cpp
  LocalizedKeyName.cpp
  PlayerCommands.cpp
  PlayerInputPacer.cpp
)

target_include_directories(rtype_client_input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_input
  PUBLIC
    SFML::Window
    rtype_engine_time
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_input)
