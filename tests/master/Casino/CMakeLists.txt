if(NOT RTYPE_MODULE_CASINO)
  return()
endif()

target_sources(master_tests PRIVATE CasinoRoutesFixture.cpp CasinoRoutesFixtures.cpp CasinoRoutesTest.cpp)

target_include_directories(master_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Jobs)
add_subdirectory(Services)
