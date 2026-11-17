add_library(rtype_client_account_session STATIC AccountSession.cpp)

target_include_directories(rtype_client_account_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_account_session
  PUBLIC
    rtype_client_connection_account
)

rtype_enable_warnings(rtype_client_account_session)
