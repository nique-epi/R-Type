#include "LuauPreHit.hpp"
#include <optional>
#include <string>
#include "EngineException.hpp"
#include "GameScriptingConstants.hpp"
#include "IncomingHit.hpp"
#include "LuauBehaviourTables.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeResult.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What pushing a pre_hit function and its hit needs, and the thread
 * created for that judgement, which ends with it. */
struct PendingJudgement {
  lua_State* machine{nullptr};
  int function{0};
  IncomingHit hit{};
  std::optional<engine::LuauThread> thread = std::nullopt;
  lua_State* threadState{nullptr};
};

/** @brief Pushes, on a thread created for the judgement, the function and the
 * table of its hit. */
void pushJudgement(void* context) {
  auto& pending = *static_cast<PendingJudgement*>(context);
  pending.threadState = pending.thread.emplace(pending.machine).state();
  lua_getref(pending.machine, pending.function);
  pushHitTable(pending.machine, pending.hit);
  lua_xmove(pending.machine, pending.threadState, 2);
}

/** @returns Whether the first value a judgement left on @p thread is
 * `false`. */
bool cancels(lua_State* thread) {
  return lua_gettop(thread) > 0 && lua_type(thread, 1) == LUA_TBOOLEAN &&
         lua_toboolean(thread, 1) == 0;
}

}  // namespace

LuauPreHit::LuauPreHit(engine::ScriptMachine& machine, int function)
    : machine_(machine), function_(function) {}

LuauPreHit::~LuauPreHit() { lua_unref(machine_.get().state(), function_); }

bool LuauPreHit::lets(const IncomingHit& hit) {
  engine::ScriptMachine& machine = machine_.get();
  if (!machine.hasTickBudget()) {
    return true;
  }
  PendingJudgement pending{
      .machine = machine.state(), .function = function_, .hit = hit};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = pushJudgement, .context = &pending});
  if (machine.resume(pending.threadState, 1) ==
      engine::LuauResumeResult::Yielded) {
    throw engine::ScriptException(std::string{WAIT_IN_PRE_HIT_MESSAGE});
  }
  return !cancels(pending.threadState);
}

}  // namespace rtype::game
