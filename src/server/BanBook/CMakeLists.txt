add_library(rtype_server_ban_book_module STATIC BanBook.cpp ServerBanBookModule.cpp)

target_include_directories(rtype_server_ban_book_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_ban_book_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
    rtype_server_bans
    rtype_server_storage_database
    rtype_server_supervised_games_module
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_ban_book_module)
