add_library(rtype_network_master_economy STATIC
  GameKopecks.cpp
  LookItems.cpp
  WalletBalances.cpp
)

target_include_directories(rtype_network_master_economy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_master_economy
  PUBLIC
    rtype_identifiers
    rtype_network_master_messages
)

rtype_enable_warnings(rtype_network_master_economy)
