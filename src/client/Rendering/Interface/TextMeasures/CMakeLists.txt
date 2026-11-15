add_library(rtype_client_text_measures STATIC TextMeasures.cpp)

target_include_directories(rtype_client_text_measures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_text_measures
  PUBLIC
    rtype_client_interface
    SFML::System
)

rtype_enable_warnings(rtype_client_text_measures)
