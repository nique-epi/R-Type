add_library(rtype_client_no_signal_card STATIC NoSignalCard.cpp)

target_include_directories(rtype_client_no_signal_card PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_no_signal_card
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_effect_interfaces
    rtype_client_glowing_call_painting
    rtype_client_outline
)

rtype_enable_warnings(rtype_client_no_signal_card)
