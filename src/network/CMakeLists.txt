find_package(asio CONFIG REQUIRED)

add_subdirectory(Exceptions)
add_subdirectory(Serialization)

add_library(rtype_network STATIC NetworkContext.cpp)

target_include_directories(rtype_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network
  PUBLIC
    asio::asio
    rtype_network_exceptions
    rtype_network_serialization
)

target_compile_definitions(rtype_network
  PUBLIC
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
)

rtype_enable_warnings(rtype_network)
