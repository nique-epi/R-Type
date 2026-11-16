add_library(rtype_client_economy STATIC
  ChipsExchange.cpp
  Kopecks.cpp
  LookItemPurchase.cpp
  PlayerPurse.cpp
)

target_include_directories(rtype_client_economy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_economy
  PUBLIC
    rtype_client_settings_values
    rtype_identifiers
    rtype_network_master_economy
)

rtype_enable_warnings(rtype_client_economy)
