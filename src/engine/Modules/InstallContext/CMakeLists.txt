add_library(rtype_engine_install_context STATIC InstallContext.cpp)

target_include_directories(rtype_engine_install_context PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_install_context
  PUBLIC
    rtype_engine_services
    rtype_engine_loop_stages
)

rtype_enable_warnings(rtype_engine_install_context)
