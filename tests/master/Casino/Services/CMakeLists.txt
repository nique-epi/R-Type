target_sources(master_tests PRIVATE RouletteDealerFixture.cpp RouletteDealerTest.cpp)

target_include_directories(master_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(master_tests PRIVATE rtype_master_casino_services)

add_subdirectory(Helpers)
