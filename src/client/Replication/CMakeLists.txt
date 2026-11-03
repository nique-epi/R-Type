add_subdirectory(Entities)
add_subdirectory(Pilots)

add_library(rtype_client_replication INTERFACE)

target_link_libraries(rtype_client_replication
  INTERFACE
    rtype_client_entities
    rtype_client_pilots
)
