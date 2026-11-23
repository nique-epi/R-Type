target_sources(master_tests PRIVATE LeaderboardRoutesFixture.cpp LeaderboardRoutesFixtures.cpp LeaderboardRoutesTest.cpp)

if(RTYPE_MODULE_WALLET)
  target_sources(master_tests PRIVATE AccountLeaderboardRoutesTest.cpp GameCreditRoutesFixture.cpp GameCreditRoutesTest.cpp)
endif()
