add_executable(r-type_server main.cpp)

target_link_libraries(r-type_server
  PRIVATE
    rtype_game
    rtype_network
)

rtype_enable_warnings(r-type_server)
