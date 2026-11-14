add_library(rtype_client_wrapped_lines STATIC WrappedLines.cpp)

target_include_directories(rtype_client_wrapped_lines PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_wrapped_lines
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_text_measures
)

rtype_enable_warnings(rtype_client_wrapped_lines)
