add_library(rtype_game_scripting_components STATIC
  LuauComponentFields.cpp
  LuauEntityHandle.cpp
  LuauScriptError.cpp
)

target_include_directories(rtype_game_scripting_components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_scripting_components
  PUBLIC
    rtype_engine_component_registry
    rtype_engine_entity_registry
    rtype_engine_scripting
  PRIVATE
    rtype_game_components
    rtype_game_scripting_constants
)

rtype_enable_warnings(rtype_game_scripting_components)
