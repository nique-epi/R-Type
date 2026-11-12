#include "LuauHitHandler.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "AppliedHit.hpp"
#include "EngineException.hpp"
#include "GameScriptingConstants.hpp"
#include "LuauBehaviourTables.hpp"
#include "LuauHitHandlerParts.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeResult.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What pushing an on_hit function and its hit needs, and the thread
 * created for that call, which ends with it. */
struct PendingReaction {
  lua_State* machine{nullptr};
  int function{0};
  AppliedHit hit{};
  std::optional<engine::LuauThread> thread = std::nullopt;
  lua_State* threadState{nullptr};
};

/** @brief Pushes, on a thread created for the call, the function and the table
 * of its hit. */
void pushReaction(void* context) {
  auto& pending = *static_cast<PendingReaction*>(context);
  pending.threadState = pending.thread.emplace(pending.machine).state();
  lua_getref(pending.machine, pending.function);
  pushAppliedHitTable(pending.machine, pending.hit);
  lua_xmove(pending.machine, pending.threadState, 2);
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

void LuauHitHandler::call(const AppliedHit& hit) {
  engine::ScriptMachine& machine = parts_.machine.get();
  PendingReaction pending{
      .machine = machine.state(), .function = parts_.function, .hit = hit};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = pushReaction, .context = &pending});
  if (machine.resume(pending.threadState, 1) ==
      engine::LuauResumeResult::Yielded) {
    throw engine::ScriptException(std::string{WAIT_IN_ON_HIT_MESSAGE});
  }
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
