#include "LuauSingleCall.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "LuauCallArguments.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeResult.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What pushing a function and its argument on a thread of its own
 * needs. */
struct PendingCall {
  lua_State* machine{nullptr};
  int function{0};
  LuauCallArguments arguments{};
  std::optional<engine::LuauThread>* thread{nullptr};
  lua_State* threadState{nullptr};
};

/** @brief Creates the thread of the call, then moves the function and its
 * argument onto it. */
void pushCall(void* context) {
  auto& pending = *static_cast<PendingCall*>(context);
  pending.threadState = pending.thread->emplace(pending.machine).state();
  lua_getref(pending.machine, pending.function);
  pending.arguments.push(pending.machine, pending.arguments.value);
  lua_xmove(pending.machine, pending.threadState, 2);
}

}  // namespace

LuauSingleCall::LuauSingleCall(engine::ScriptMachine& machine, int function)
    : machine_(machine), function_(function) {}

void LuauSingleCall::run(LuauCallArguments arguments,
                         std::string_view waitRefusal) {
  engine::ScriptMachine& machine = machine_.get();
  PendingCall pending{.machine = machine.state(),
                      .function = function_,
                      .arguments = arguments,
                      .thread = &thread_};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = pushCall, .context = &pending});
  if (machine.resume(pending.threadState, 1) ==
      engine::LuauResumeResult::Yielded) {
    throw engine::ScriptException(std::string{waitRefusal});
  }
}

lua_State* LuauSingleCall::thread() const {
  return thread_.has_value() ? thread_->state() : nullptr;
}

}  // namespace rtype::game
