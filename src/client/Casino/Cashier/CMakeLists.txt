add_subdirectory(Exchange)
add_subdirectory(Room)
add_subdirectory(Screen)

add_library(rtype_client_casino_cashier INTERFACE)

target_link_libraries(rtype_client_casino_cashier
  INTERFACE
    rtype_client_casino_cashier_exchange
    rtype_client_casino_cashier_room
    rtype_client_casino_cashier_screen
)
