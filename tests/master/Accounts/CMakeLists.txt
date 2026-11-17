if(NOT RTYPE_MODULE_ACCOUNTS)
  return()
endif()

target_sources(master_tests PRIVATE AccountsRoutesFixture.cpp AccountsRoutesFixtures.cpp AccountsRoutesTest.cpp SiteAccountsRoutesFixture.cpp SiteAccountsRoutesTest.cpp)

target_link_libraries(master_tests PRIVATE rtype_master_accounts_services rtype_network_hashing)

add_subdirectory(Services)
