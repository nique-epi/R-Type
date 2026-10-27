add_library(rtype_client_end_choices STATIC EndChoices.cpp)

target_include_directories(rtype_client_end_choices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_end_choices
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_end_choices)
