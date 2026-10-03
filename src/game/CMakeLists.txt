add_library(rtype_game STATIC World.cpp)

target_include_directories(rtype_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game PUBLIC rtype_engine)

rtype_enable_warnings(rtype_game)
