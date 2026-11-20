add_subdirectory(Picture)
add_subdirectory(Screen)

add_library(rtype_client_casino_hall INTERFACE)

target_link_libraries(rtype_client_casino_hall
  INTERFACE
    rtype_client_casino_hall_picture
    rtype_client_casino_hall_screen
)
