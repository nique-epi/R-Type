find_package(asio CONFIG REQUIRED)

add_executable(r-type_server main.cpp)

target_link_libraries(r-type_server PRIVATE asio::asio)

target_compile_definitions(r-type_server
  PRIVATE
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
)

rtype_enable_warnings(r-type_server)
