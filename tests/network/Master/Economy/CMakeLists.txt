target_sources(network_master_tests PRIVATE GameKopecksTest.cpp LookItemsTest.cpp
  WalletBalancesTest.cpp)
target_include_directories(network_master_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
