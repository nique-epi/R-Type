add_subdirectory(Pack)
add_subdirectory(Delivery)

add_library(rtype_network_content INTERFACE)

target_link_libraries(rtype_network_content
  INTERFACE
    rtype_network_content_pack
    rtype_network_content_delivery
)
