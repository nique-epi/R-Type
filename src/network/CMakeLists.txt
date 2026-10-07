find_package(asio CONFIG REQUIRED)

add_library(rtype_network_asio INTERFACE)

target_link_libraries(rtype_network_asio INTERFACE asio::asio)

target_compile_definitions(rtype_network_asio
  INTERFACE
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
)

add_subdirectory(NetworkContext)

add_library(rtype_network INTERFACE)

target_include_directories(rtype_network INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network
  INTERFACE
    rtype_network_context
)
