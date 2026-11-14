#include "LuauHitHandler.hpp"
#include <string>
#include <string_view>
#include <utility>
#include "AppliedHit.hpp"
#include "EngineException.hpp"
#include "GameScriptingConstants.hpp"
#include "LuauBehaviourTables.hpp"
#include "LuauCallArguments.hpp"
#include "LuauHitHandlerParts.hpp"
#include "LuauSingleCall.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief Pushes the table of the AppliedHit @p value. */
void pushAppliedHit(lua_State* machine, const void* value) {
  pushAppliedHitTable(machine, *static_cast<const AppliedHit*>(value));
}

}  // namespace

LuauHitHandler::LuauHitHandler(LuauHitHandlerParts parts)
    : parts_(std::move(parts)) {}

LuauHitHandler::~LuauHitHandler() {
  lua_unref(parts_.machine.get().state(), parts_.function);
}

std::string_view LuauHitHandler::name() const { return parts_.name; }

void LuauHitHandler::onHit(const AppliedHit& hit) {
  if (!parts_.machine.get().hasTickBudget()) {
    return;
  }
  try {
    call(hit);
  } catch (const engine::ScriptException& error) {
    reportOnce(error.what());
  }
}

void LuauHitHandler::call(const AppliedHit& hit) const {
  LuauSingleCall call{parts_.machine.get(), parts_.function};
  call.run(LuauCallArguments{.push = pushAppliedHit, .value = &hit},
           WAIT_IN_ON_HIT_MESSAGE);
}

void LuauHitHandler::reportOnce(std::string_view reason) {
  if (failureReported_) {
    return;
  }
  failureReported_ = true;
  parts_.reportFailure(std::string{ON_HIT_FAILURE_PREFIX} + parts_.name +
                       std::string{ON_HIT_FAILURE_SEPARATOR} +
                       std::string{reason});
}

}  // namespace rtype::game
