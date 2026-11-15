add_subdirectory(Painting)
add_subdirectory(Entrance)
add_subdirectory(Cashier)
add_subdirectory(Roulette)

add_library(rtype_client_casino_module STATIC
  ChipSettlement.cpp
  ClientCasinoModule.cpp
)

target_include_directories(rtype_client_casino_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_module
  PUBLIC
    rtype_client_connection_casino
    rtype_client_settings_module
    rtype_engine_modules
  PRIVATE
    rtype_client_casino_cashier
    rtype_client_casino_entrance
    rtype_client_constants
    rtype_client_screens_module
)

rtype_enable_warnings(rtype_client_casino_module)
