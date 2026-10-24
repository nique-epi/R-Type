add_library(rtype_client_flash_budget STATIC FlashBudget.cpp)

target_include_directories(rtype_client_flash_budget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flash_budget
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_flash_budget)
