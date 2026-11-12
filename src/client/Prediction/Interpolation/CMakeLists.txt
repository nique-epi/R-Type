add_library(rtype_client_prediction_interpolation STATIC
  ArrivalReveal.cpp
  InterpolationClock.cpp
  InterpolationTally.cpp
  PositionInterpolation.cpp
)

target_include_directories(rtype_client_prediction_interpolation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_prediction_interpolation
  PUBLIC
    rtype_client_entities
    rtype_engine
    rtype_engine_time
    rtype_game_components
)

rtype_enable_warnings(rtype_client_prediction_interpolation)
