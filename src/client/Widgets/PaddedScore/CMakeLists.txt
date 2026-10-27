add_library(rtype_client_padded_score STATIC PaddedScore.cpp)

target_include_directories(rtype_client_padded_score PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_padded_score
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_padded_score)
