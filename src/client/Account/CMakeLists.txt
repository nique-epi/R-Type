add_subdirectory(Session)
add_subdirectory(Wallet)

add_library(rtype_client_account_module STATIC ClientAccountModule.cpp)

target_include_directories(rtype_client_account_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_account_module
  PUBLIC
    rtype_client_account_session
    rtype_client_account_wallet
    rtype_engine_install_context
    rtype_engine_modules
  PRIVATE
    rtype_client_constants
    rtype_client_platform
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_account_module)
