add_library(rtype_client_account_casino STATIC
  CasinoBetsDesk.cpp
  CasinoChannel.cpp
  CasinoRoomWatch.cpp
  CasinoSeatKeeper.cpp
)

target_include_directories(rtype_client_account_casino PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_account_casino
  PUBLIC
    rtype_client_account_session
    rtype_client_account_wallet
    rtype_client_connection_casino
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_account_casino)
