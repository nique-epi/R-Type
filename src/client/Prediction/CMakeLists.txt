add_subdirectory(Interpolation)
add_subdirectory(OwnShip)
add_subdirectory(OwnMissiles)

add_library(rtype_client_prediction INTERFACE)

target_link_libraries(rtype_client_prediction
  INTERFACE
    rtype_client_prediction_interpolation
    rtype_client_prediction_own_missiles
    rtype_client_prediction_own_ship
)
