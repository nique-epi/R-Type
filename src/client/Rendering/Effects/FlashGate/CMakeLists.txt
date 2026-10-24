add_library(rtype_client_flash_gate STATIC FlashGate.cpp)

target_include_directories(rtype_client_flash_gate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flash_gate
  PUBLIC
    rtype_client_effect_interfaces
    rtype_client_flash_budget
    rtype_engine
  PRIVATE
    rtype_client_camera_shake
)

rtype_enable_warnings(rtype_client_flash_gate)
