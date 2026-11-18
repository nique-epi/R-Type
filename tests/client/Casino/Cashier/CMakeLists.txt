target_sources(client_screen_tests PRIVATE
  CashierAmountTest.cpp
  CashierRulesTest.cpp
  CashierScreenFixture.cpp
  CashierScreenTest.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client_screen_tests PRIVATE rtype_client_casino_cashier)
