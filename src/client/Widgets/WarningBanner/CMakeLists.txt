add_library(rtype_client_warning_banner STATIC WarningBanner.cpp)

target_include_directories(rtype_client_warning_banner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_warning_banner
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_easing
    rtype_client_ground_effect_interfaces
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_warning_banner)
