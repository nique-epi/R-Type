add_library(rtype_client_spoken_line STATIC SpokenLine.cpp)

target_include_directories(rtype_client_spoken_line PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_spoken_line
  PUBLIC
    rtype_client_audio_contract_sounds
    rtype_client_speech_lines
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_spoken_line)
