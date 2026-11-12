target_sources(client_screen_tests PRIVATE
  ShopMusicFixture.cpp
  ShopMusicTest.cpp
  ShopScreenFixture.cpp
  ShopScreenTest.cpp
  ShopStatusTest.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client_screen_tests PRIVATE rtype_client_shop_module rtype_client_shop_painting)
target_link_libraries(client_module_tests PRIVATE rtype_client_shop_module)

add_subdirectory(Shelves)
add_subdirectory(Painting)
