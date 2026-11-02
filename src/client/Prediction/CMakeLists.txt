add_library(rtype_client_prediction STATIC
  OwnShip.cpp
  OwnShipPredictor.cpp
  PredictedShipPlacement.cpp
  ShipPrediction.cpp
)

target_include_directories(rtype_client_prediction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_prediction
  PUBLIC
    rtype_client_connection
    rtype_client_entity_replicator
    rtype_engine
    rtype_engine_time
    rtype_game_components
    rtype_game_ship_control
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_prediction)
