#include "LuauBehaviour.hpp"
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
#include "IBodyOrders.hpp"
#include "Intent.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauBodyMemory.hpp"
#include "LuauSetting.hpp"
#include "LuauStepState.hpp"
#include "LuauView.hpp"
#include "Routine.hpp"
#include "ScriptMachine.hpp"
#include "ScriptProtection.hpp"
#include "ScriptStepEnd.hpp"
#include "ScriptThread.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What the first half of a step needs. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct StepStart {
  std::reference_wrapper<const LuauBehaviourParts> parts;
  std::reference_wrapper<LuauBodyMemory> memory;
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

/** @brief Pushes on the thread of a body, created at its first step, what
 * resuming it needs: a fresh view, after the function when it does not wait. */
void pushStep(void* context) {
  auto& start = *static_cast<StepStart*>(context);
  LuauBodyMemory& memory = start.memory.get();
  lua_State* const machine = start.parts.get().machine.get().state();
  if (!memory.thread.has_value()) {
    memory.thread.emplace(machine);
  }
  lua_State* const thread = memory.thread->state();
  start.thread = thread;
  if (!memory.waiting) {
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
                               const LuauStepState& step) {
  if (end == engine::ScriptStepEnd::Finished) {
    return 0;
  }
  if (!step.waitTicks.has_value()) {
    throw engine::ScriptException(
        "a body may only yield through behaviours.wait");
  }
  return *step.waitTicks;
}

}  // namespace

LuauBehaviour::LuauBehaviour(LuauBehaviourParts parts)
    : parts_(std::move(parts)) {
  settingNames_.reserve(parts_.settings.size());
  settings_.reserve(parts_.settings.size());
  for (const LuauSetting& setting : parts_.settings) {
    settingNames_.push_back(setting.name);
  }
  for (std::size_t index = 0; index < parts_.settings.size(); ++index) {
    settings_.push_back(BehaviourSetting{
        .name = settingNames_[index], .range = parts_.settings[index].range});
  }
}

std::string_view LuauBehaviour::name() const { return parts_.name; }

std::span<const BehaviourSetting> LuauBehaviour::settings() const {
  return settings_;
}

Intent LuauBehaviour::steer(const BehaviourView& view) const {
  std::shared_ptr<IBehaviourMemory> forgotten;
  std::shared_ptr<IBehaviourMemory>& memory =
      view.memory == nullptr ? forgotten : *view.memory;
  auto* found = dynamic_cast<LuauBodyMemory*>(memory.get());
  if (found == nullptr) {
    auto fresh = std::make_shared<LuauBodyMemory>();
    found = fresh.get();
    memory = std::move(fresh);
  }
  try {
    return stepOf(*found, view);
  } catch (const engine::ScriptException& error) {
    stopAfter(*found, error);
    removeStopped(view);
    return found->routine.held();
  }
}

Intent LuauBehaviour::stepOf(LuauBodyMemory& memory,
                             const BehaviourView& view) const {
  if (memory.routine.stopped()) {
    removeStopped(view);
  }
  const std::optional<Intent> held = memory.routine.heldThisTick();
  if (held.has_value()) {
    return *held;
  }
  engine::ScriptMachine& machine = parts_.machine.get();
  if (!machine.hasTickBudget()) {
    return memory.routine.held();
  }
  StepStart start{.parts = parts_,
                  .memory = memory,
                  .view = view,
                  .settingNames = settingNames_};
  engine::runProtected(machine.state(),
                       engine::ScriptWork{.run = pushStep, .context = &start});
  LuauStepState& step = parts_.step.get();
  step.stepping = start.thread;
  step.waitTicks.reset();
  const engine::ScriptStepEnd end = machine.resume(start.thread, 1);
  step.stepping = nullptr;
  const std::uint32_t waitTicks = ticksToWaitAfter(end, step);
  memory.waiting = end == engine::ScriptStepEnd::Yielded;
  StepResult result{.machine = machine.state(), .thread = start.thread};
  engine::runProtected(machine.state(),
                       engine::ScriptWork{.run = readStep, .context = &result});
  return memory.routine.stepped(result.intent, waitTicks);
}

void LuauBehaviour::stopAfter(LuauBodyMemory& memory,
                              const engine::ScriptException& error) const {
  parts_.step.get().stepping = nullptr;
  memory.thread.reset();
  memory.waiting = false;
  memory.routine.stop();
  if (!failureReported_) {
    failureReported_ = true;
    parts_.reportFailure("the behaviour " + parts_.name +
                         " failed: " + error.what());
  }
}

void LuauBehaviour::removeStopped(const BehaviourView& view) const {
  IBodyOrders* const orders = parts_.step.get().orders;
  if (view.networkId == 0 || orders == nullptr) {
    return;
  }
  try {
    orders->destroy(view.networkId);
  } catch (const engine::ScriptException&) {
    return;
  }
}

}  // namespace rtype::game
