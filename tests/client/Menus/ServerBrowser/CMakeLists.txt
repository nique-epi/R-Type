target_sources(client_screen_tests PRIVATE
  GamePanelTest.cpp
  NewGameDialogFixture.cpp
  NewGameDialogTest.cpp
  ServerMapScreenFixture.cpp
  LargestServerMapFixture.cpp
  ServerMapScreenFixtures.cpp
  ServerMapScreenTest.cpp
  SessionEndWatchTest.cpp
  ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingGameCatalog.cpp
  ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingServerCatalog.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client_screen_tests PRIVATE rtype_client_menus_server_browser)
