if(NOT RTYPE_MODULE_WALLET)
  return()
endif()

target_sources(master_tests PRIVATE GameTicketRoutesTest.cpp WalletRoutesFixture.cpp WalletRoutesFixtures.cpp WalletRoutesTest.cpp)

target_include_directories(master_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(master_tests PRIVATE rtype_master_wallet_services)

add_subdirectory(Services)
