add_library(rtype_engine_collision_masks STATIC PixelMask.cpp MaskOverlap.cpp)

target_include_directories(rtype_engine_collision_masks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_collision_masks
  PRIVATE
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_collision_masks)
