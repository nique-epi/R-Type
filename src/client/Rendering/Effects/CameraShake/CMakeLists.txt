add_library(rtype_client_camera_shake STATIC CameraShake.cpp)

target_include_directories(rtype_client_camera_shake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_camera_shake
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_camera_shake)
