add_library(rtype_client_book_painting STATIC BookPainting.cpp)

target_include_directories(rtype_client_book_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_book_painting
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_easing
    rtype_client_entity_sprites
)

rtype_enable_warnings(rtype_client_book_painting)
