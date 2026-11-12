add_library(rtype_engine_collision_grid STATIC UniformGrid.cpp)

target_include_directories(rtype_engine_collision_grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_collision_grid
  PUBLIC
    rtype_engine_collision_shapes
  PRIVATE
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_collision_grid)
