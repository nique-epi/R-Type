add_library(rtype_game_contacts STATIC ShapeContacts.cpp)

target_include_directories(rtype_game_contacts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_contacts
  PUBLIC
    rtype_engine_collision
    rtype_game_components
)

rtype_enable_warnings(rtype_game_contacts)
