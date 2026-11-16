add_library(rtype_client_speech_lines STATIC SpeechLines.cpp)

target_include_directories(rtype_client_speech_lines PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_speech_lines
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_key_cap
    rtype_client_typing
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_speech_lines)
