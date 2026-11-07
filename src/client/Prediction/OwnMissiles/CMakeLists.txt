add_library(rtype_client_prediction_own_missiles STATIC
  OwnMissiles.cpp
  OwnPrediction.cpp
  PredictedGun.cpp
)

target_include_directories(rtype_client_prediction_own_missiles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_prediction_own_missiles
  PUBLIC
    rtype_client_entities
    rtype_client_prediction_own_ship
    rtype_engine
    rtype_engine_time
    rtype_game_components
    rtype_network_protocol
  PRIVATE
    rtype_client_entity_sprites
    rtype_engine_collision
    rtype_game_spawning
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_prediction_own_missiles)
