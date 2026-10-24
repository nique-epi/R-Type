add_library(rtype_client_player_preferences STATIC PlayerPreferences.cpp)

target_include_directories(rtype_client_player_preferences PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_player_preferences
  PUBLIC
    rtype_client_rendering_interfaces
  PRIVATE
    rtype_client_settings
)

rtype_enable_warnings(rtype_client_player_preferences)
