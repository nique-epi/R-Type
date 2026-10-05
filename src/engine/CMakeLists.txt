add_library(rtype_engine STATIC
  EntityRegistry.cpp
  Exceptions/EngineException.cpp
  Time/FixedTimestep.cpp
  Time/SystemClock.cpp
)

target_include_directories(rtype_engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Exceptions
    ${CMAKE_CURRENT_SOURCE_DIR}/Time
)

rtype_enable_warnings(rtype_engine)
