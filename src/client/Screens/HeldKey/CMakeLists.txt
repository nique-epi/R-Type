add_library(rtype_client_held_key STATIC HeldKey.cpp)

target_include_directories(rtype_client_held_key PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_held_key
  PUBLIC
    SFML::Window
)

rtype_enable_warnings(rtype_client_held_key)
