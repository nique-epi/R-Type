#include "ScriptedBehaviour.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "EngineException.hpp"
#include "IBehaviourMemory.hpp"
#include "Intent.hpp"
#include "ScriptMachine.hpp"
#include "ScriptProtection.hpp"
#include "ScriptStepEnd.hpp"
#include "ScriptThread.hpp"
#include "ScriptView.hpp"
#include "ScriptWaitState.hpp"
#include "ScriptedBehaviourParts.hpp"
#include "ScriptedRoutine.hpp"
#include "ScriptedSetting.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What the first half of a step needs. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct StepStart {
  std::reference_wrapper<const ScriptedBehaviourParts> parts;
  std::reference_wrapper<ScriptedRoutine> routine;
  std::reference_wrapper<const BehaviourView> view;
  std::span<const std::string> settingNames;
  lua_State* thread{nullptr};
};

/** @brief What the end of a step reads, and what it found. */
struct StepResult {
  lua_State* machine{nullptr};
  lua_State* thread{nullptr};
  Intent intent{.steered = true};
};

/** @brief Pushes on the thread of a routine, created at its first step, what
 * resuming it needs: a fresh view, after the function when it does not wait. */
void pushStep(void* context) {
  auto& start = *static_cast<StepStart*>(context);
  ScriptedRoutine& routine = start.routine.get();
  lua_State* const machine = start.parts.get().machine.get().state();
  if (!routine.thread.has_value()) {
    routine.thread.emplace(machine);
  }
  lua_State* const thread = routine.thread->state();
  start.thread = thread;
  if (routine.ticksLeft == 0) {
    lua_getref(machine, start.parts.get().steer);
    lua_xmove(machine, thread, 1);
  }
  pushBehaviourView(machine, start.view.get(), start.settingNames);
  lua_xmove(machine, thread, 1);
}

/** @brief Reads the intent a step left first on the stack of its thread, and
 * empties that stack for the next step. */
void readStep(void* context) {
  auto& result = *static_cast<StepResult*>(context);
  if (lua_gettop(result.thread) > 0) {
    lua_xpush(result.thread, result.machine, 1);
    result.intent = intentAt(result.machine, -1);
    lua_settop(result.thread, 0);
  }
}

/** @returns The ticks a body waits after a step that ended with @p end: 0
 * when its function returned. @throws engine::ScriptException when it yielded
 * without behaviours.wait. */
std::uint32_t ticksToWaitAfter(engine::ScriptStepEnd end,
                               const ScriptWaitState& waits) {
  if (end == engine::ScriptStepEnd::Finished) {
    return 0;
  }
  if (!waits.waitTicks.has_value()) {
    throw engine::ScriptException(
        "a body may only yield through behaviours.wait");
  }
  return *waits.waitTicks;
}

/** @returns @p intent without what applies once: an action started or ended,
 * and a jump. */
Intent withoutOneTimeRequests(Intent intent) {
  intent.startsAction = std::nullopt;
  intent.endsAction = false;
  intent.jumpsTo = std::nullopt;
  return intent;
}

}  // namespace

ScriptedBehaviour::ScriptedBehaviour(ScriptedBehaviourParts parts)
    : parts_(std::move(parts)) {
  settingNames_.reserve(parts_.settings.size());
  settings_.reserve(parts_.settings.size());
  for (const ScriptedSetting& setting : parts_.settings) {
    settingNames_.push_back(setting.name);
  }
  for (std::size_t index = 0; index < parts_.settings.size(); ++index) {
    settings_.push_back(BehaviourSetting{
        .name = settingNames_[index], .range = parts_.settings[index].range});
  }
}

std::string_view ScriptedBehaviour::name() const { return parts_.name; }

std::span<const BehaviourSetting> ScriptedBehaviour::settings() const {
  return settings_;
}

Intent ScriptedBehaviour::steer(const BehaviourView& view) const {
  std::shared_ptr<IBehaviourMemory> forgotten;
  std::shared_ptr<IBehaviourMemory>& memory =
      view.memory == nullptr ? forgotten : *view.memory;
  auto* found = dynamic_cast<ScriptedRoutine*>(memory.get());
  if (found == nullptr) {
    auto fresh = std::make_shared<ScriptedRoutine>();
    found = fresh.get();
    memory = std::move(fresh);
  }
  ScriptedRoutine& routine = *found;
  try {
    return stepOf(routine, view);
  } catch (const engine::ScriptException& error) {
    parts_.waits.get().stepping = nullptr;
    routine.thread.reset();
    routine.stopped = true;
    if (!failureReported_) {
      failureReported_ = true;
      parts_.reportFailure("the behaviour " + parts_.name +
                           " failed: " + error.what());
    }
    removeStopped(view);
    return Intent{.steered = true};
  }
}

Intent ScriptedBehaviour::stepOf(ScriptedRoutine& routine,
                                 const BehaviourView& view) const {
  if (routine.stopped) {
    removeStopped(view);
    return Intent{.steered = true};
  }
  if (routine.ticksLeft > 1) {
    --routine.ticksLeft;
    return routine.held;
  }
  engine::ScriptMachine& machine = parts_.machine.get();
  if (!machine.hasTickBudget()) {
    return routine.held;
  }
  StepStart start{.parts = parts_,
                  .routine = routine,
                  .view = view,
                  .settingNames = settingNames_};
  engine::runProtected(machine.state(),
                       engine::ScriptWork{.run = pushStep, .context = &start});
  lua_State* const thread = start.thread;
  ScriptWaitState& waits = parts_.waits.get();
  waits.stepping = thread;
  waits.waitTicks.reset();
  const engine::ScriptStepEnd end = machine.resume(thread, 1);
  waits.stepping = nullptr;
  routine.ticksLeft = ticksToWaitAfter(end, waits);
  StepResult result{.machine = machine.state(), .thread = thread};
  engine::runProtected(machine.state(),
                       engine::ScriptWork{.run = readStep, .context = &result});
  routine.held = withoutOneTimeRequests(result.intent);
  return result.intent;
}

void ScriptedBehaviour::removeStopped(const BehaviourView& view) const {
  if (view.networkId == 0 || !parts_.removeBody) {
    return;
  }
  try {
    parts_.removeBody(view.networkId);
  } catch (const engine::ScriptException&) {
    return;
  }
}

}  // namespace rtype::game
