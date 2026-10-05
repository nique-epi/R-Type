add_subdirectory(EntityRegistry)

add_library(rtype_engine INTERFACE)

target_include_directories(rtype_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine INTERFACE rtype_engine_entity_registry)
