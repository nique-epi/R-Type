#include "ScriptMachineFixtures.hpp"
#include <array>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "ScriptFunction.hpp"
#include "ScriptMachine.hpp"
#include "ScriptMachineTestConstants.hpp"
#include "ScriptSource.hpp"
#include "lua.h"

namespace {

ScriptProbe& probeOf(lua_State* state) {
  return *static_cast<ScriptProbe*>(lua_touserdata(state, lua_upvalueindex(1)));
}

int recordNumber(lua_State* state) {
  probeOf(state).recorded = lua_tonumber(state, 1);
  return 0;
}

int keepFunction(lua_State* state) {
  probeOf(state).keptFunction = lua_ref(state, 1);
  return 0;
}

}  // namespace

std::array<rtype::engine::ScriptFunction, 2> probeFunctions() {
  return {
      rtype::engine::ScriptFunction{.name = "record", .function = recordNumber},
      rtype::engine::ScriptFunction{.name = "keep", .function = keepFunction}};
}

rtype::engine::ScriptSource scriptOf(std::string_view text) {
  return rtype::engine::ScriptSource{.name = std::string{SCRIPT_NAME},
                                     .text = std::string{text}};
}

std::string errorOfRunning(rtype::engine::ScriptMachine& machine,
                           const rtype::engine::ScriptSource& source) {
  try {
    machine.run(source);
  } catch (const rtype::engine::ScriptException& error) {
    return error.what();
  }
  return {};
}

double resultOfKeptFunction(rtype::engine::ScriptMachine& machine,
                            const ScriptProbe& probe, double argument) {
  lua_State* const state = machine.state();
  lua_getref(state, probe.keptFunction);
  lua_pushnumber(state, argument);
  machine.call(1, 1);
  const double result = lua_tonumber(state, -1);
  lua_pop(state, 1);
  return result;
}
