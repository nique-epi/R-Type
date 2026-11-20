target_sources(client_screen_tests PRIVATE
  ${RTYPE_TEST_SOURCE_DIR}/client/Rendering/SheetPixels.cpp
)
target_include_directories(client_screen_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${RTYPE_TEST_SOURCE_DIR}/client/Rendering
)
target_compile_definitions(client_screen_tests PRIVATE RTYPE_SOURCE_ASSETS_FOLDER="${PROJECT_SOURCE_DIR}/assets")
target_link_libraries(client_screen_tests PRIVATE rtype_client_casino_module)
add_subdirectory(Channel)
add_subdirectory(Entrance)
add_subdirectory(Hall)
add_subdirectory(Painting)
add_subdirectory(Cashier)
add_subdirectory(Roulette)
