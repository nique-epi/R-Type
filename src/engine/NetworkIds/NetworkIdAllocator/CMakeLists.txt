add_library(rtype_engine_network_id_allocator STATIC NetworkIdAllocator.cpp)

target_include_directories(rtype_engine_network_id_allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_network_id_allocator
  PUBLIC
    rtype_engine_network_id_constants
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_network_id_allocator)
