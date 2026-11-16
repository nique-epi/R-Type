target_sources(master_tests PRIVATE WalletServiceFixture.cpp WalletServiceTest.cpp)

target_include_directories(master_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Helpers)
