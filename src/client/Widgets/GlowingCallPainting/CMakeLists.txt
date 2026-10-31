add_library(rtype_client_glowing_call_painting STATIC GlowingCallPainting.cpp)

target_include_directories(rtype_client_glowing_call_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_glowing_call_painting
  PUBLIC
    rtype_client_interface
    rtype_client_widgets
  PRIVATE
    rtype_client_effect_interfaces
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_glowing_call_painting)
