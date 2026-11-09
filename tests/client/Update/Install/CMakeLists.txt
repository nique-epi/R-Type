target_sources(client_update_tests PRIVATE InstallFixtures.cpp)
target_include_directories(client_update_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Platform)
add_subdirectory(Notes)
