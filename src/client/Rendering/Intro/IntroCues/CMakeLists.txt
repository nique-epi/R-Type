add_library(rtype_client_intro_cues STATIC IntroCues.cpp)

target_include_directories(rtype_client_intro_cues PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_cues
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_intro_mob_paths
    rtype_client_intro_timeline
)

rtype_enable_warnings(rtype_client_intro_cues)
