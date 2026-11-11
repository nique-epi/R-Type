find_package(unofficial-luau CONFIG REQUIRED)

add_library(rtype_engine_scripting STATIC ScriptMachine.cpp LuauProtectedCall.cpp LuauThread.cpp ScriptWait.cpp)

target_include_directories(rtype_engine_scripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_scripting
  PUBLIC
    rtype_engine_exceptions
    unofficial::luau::Luau.VM
  PRIVATE
    rtype_engine_time
    unofficial::luau::Luau.Compiler
)

rtype_enable_warnings(rtype_engine_scripting)
