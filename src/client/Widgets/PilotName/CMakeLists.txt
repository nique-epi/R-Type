add_library(rtype_client_pilot_name STATIC PilotName.cpp)

target_include_directories(rtype_client_pilot_name PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pilot_name
  PRIVATE
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_pilot_name)
