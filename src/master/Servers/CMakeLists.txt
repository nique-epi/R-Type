add_subdirectory(Models)
add_subdirectory(Repositories)
add_subdirectory(Services)
add_subdirectory(Jobs)
add_subdirectory(Controllers)

add_library(rtype_master_servers_module STATIC
  ServersModule.cpp
)

target_include_directories(rtype_master_servers_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_module
  PUBLIC
    rtype_engine_modules
    rtype_master_servers_controllers
    rtype_master_servers_jobs
  PRIVATE
    rtype_master_constants
    rtype_master_http_module
    rtype_master_storage_module
    rtype_network_master_routes
)

rtype_enable_warnings(rtype_master_servers_module)
