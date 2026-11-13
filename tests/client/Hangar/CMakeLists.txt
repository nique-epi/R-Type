target_sources(client_screen_tests PRIVATE
  HangarScreenFixture.cpp
  HangarScreenFixtures.cpp
  HangarScreenTest.cpp
  HangarScrollFixture.cpp
  HangarScrollTest.cpp
  WonItemsTest.cpp
  ReducedMotionHangarFixture.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client_screen_tests PRIVATE rtype_client_hangar_module)

add_subdirectory(Locker)
add_subdirectory(Dock)
