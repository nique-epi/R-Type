add_library(rtype_network_content_delivery STATIC ContentPackRoute.cpp)

target_include_directories(rtype_network_content_delivery PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_content_delivery
  PUBLIC
    rtype_network_content_pack
    rtype_network_master_routes
)

rtype_enable_warnings(rtype_network_content_delivery)
