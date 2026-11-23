add_library(rtype_client_account_session STATIC
  AccountSession.cpp
  SessionTokenFile.cpp
)

target_include_directories(rtype_client_account_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_account_session
  PUBLIC
    rtype_client_connection_account
    rtype_logging
  PRIVATE
    rtype_client_settings_storage
)

rtype_enable_warnings(rtype_client_account_session)
