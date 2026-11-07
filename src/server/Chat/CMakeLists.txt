if(NOT RTYPE_MODULE_CHAT)
  return()
endif()

add_library(rtype_server_chat_module STATIC
  ChatModule.cpp
  ChatRelay.cpp
)

target_include_directories(rtype_server_chat_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_chat_module
  PUBLIC
    rtype_engine_install_context
    rtype_network_chat_line
    rtype_server_instances
  PRIVATE
    rtype_network_player_chat_line
    rtype_server_sessions_roster
    rtype_server_sessions_tracking
)

rtype_enable_warnings(rtype_server_chat_module)
