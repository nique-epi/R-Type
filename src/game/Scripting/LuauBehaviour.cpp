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
#include "HeldIntent.hpp"
#include "IBehaviourState.hpp"
#include "IEntityOrders.hpp"
#include "Intent.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauBehaviourState.hpp"
#include "LuauBehaviourTables.hpp"
#include "LuauEntityHandle.hpp"
#include "LuauEventSubscriptions.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeContext.hpp"
#include "LuauResumeResult.hpp"
#include "LuauSetting.hpp"
#include "LuauSteppedBody.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What resuming the thread of an entity needs. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ResumeInputs {
  std::reference_wrapper<const LuauBehaviourParts> parts;
  std::reference_wrapper<LuauBehaviourState> behaviourState;
  std::reference_wrapper<const BehaviourView> view;
  std::span<const std::string> settingNames;
  lua_State* thread{nullptr};
};

/** @brief What reading the end of a resume needs, and the intent it found. */
struct ResumeOutputs {
  lua_State* machine{nullptr};
  lua_State* thread{nullptr};
  Intent intent{.steered = true};
};

/** @returns The handle of the entity of @p behaviourState, made at its first
 * resume on @p machine. */
const LuauEntityHandle& handleOf(LuauBehaviourState& behaviourState,
                                 lua_State* machine,
                                 LuauSteppedBody& steppedBody) {
  if (!behaviourState.handle.has_value()) {
    return behaviourState.handle.emplace(machine, steppedBody);
  }
  return *behaviourState.handle;
}

/** @brief Pushes on the thread of an entity, created at its first resume, what
 * resuming it needs: its handle, holding a fresh view, after the function when
 * it does not wait. */
void pushResumeInputs(void* context) {
  auto& start = *static_cast<ResumeInputs*>(context);
  LuauBehaviourState& behaviourState = start.behaviourState.get();
  const LuauBehaviourParts& parts = start.parts.get();
  lua_State* const machine = parts.machine.get().state();
  if (!behaviourState.thread.has_value()) {
    behaviourState.thread.emplace(machine);
  }
  lua_State* const thread = behaviourState.thread->state();
  start.thread = thread;
  if (!behaviourState.waiting) {
    lua_getref(machine, parts.steer);
    lua_xmove(machine, thread, 1);
  }
  pushViewTable(machine, start.view.get(), start.settingNames);
  handleOf(behaviourState, machine, parts.resumeContext.get().steppedBody)
      .pushWithView();
  lua_xmove(machine, thread, 1);
}

/** @brief Tells the functions the scripts call that the entity of @p start
 * steps: its thread, its handle and its entity, and its subscriptions. */
void openStep(const LuauBehaviourParts& parts, const ResumeInputs& start) {
  LuauResumeContext& context = parts.resumeContext.get();
  LuauBehaviourState& behaviourState = start.behaviourState.get();
  const BehaviourView& view = start.view.get();
  context.resumedThread = start.thread;
  context.waitTicks.reset();
  context.steppedBody.handle = behaviourState.handle.has_value()
                                   ? behaviourState.handle->slot()
                                   : nullptr;
  context.steppedBody.entity = view.entity;
  parts.eventBindings.get().setSubscriber(&behaviourState.subscriptions);
}

/** @brief Tells the functions the scripts call that no entity steps. */
void closeStep(const LuauBehaviourParts& parts) {
  LuauResumeContext& context = parts.resumeContext.get();
  context.resumedThread = nullptr;
  context.steppedBody.handle = nullptr;
  context.steppedBody.entity = engine::Entity{};
  parts.eventBindings.get().setSubscriber(nullptr);
}

/** @brief Reads the intent a resume left first on the stack of its thread, and
 * empties that stack for the next resume. */
void readResumeOutputs(void* context) {
  auto& result = *static_cast<ResumeOutputs*>(context);
  if (lua_gettop(result.thread) > 0) {
    lua_xpush(result.thread, result.machine, 1);
    result.intent = intentFromTable(result.machine, -1);
    lua_settop(result.thread, 0);
  }
}

/** @returns The ticks an entity waits after a resume that ended with @p end: 0
 * when its function returned. @throws engine::ScriptException when it yielded
 * without behaviours.wait. */
std::uint32_t ticksToWaitAfter(engine::LuauResumeResult end,
                               const LuauResumeContext& context) {
  if (end == engine::LuauResumeResult::Returned) {
    return 0;
  }
  if (!context.waitTicks.has_value()) {
    throw engine::ScriptException(
        "a body may only yield through behaviours.wait");
  }
  return *context.waitTicks;
}

/** @throws engine::ScriptException, holding why, when a function of
 * @p subscriptions failed since the last step of its entity. */
void refuseAfterAFailedSubscribedFunction(
    const LuauEventSubscriptions& subscriptions) {
  const std::optional<std::string>& failure = subscriptions.failure();
  if (failure.has_value()) {
    throw engine::ScriptException(*failure);
  }
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
  std::shared_ptr<IBehaviourState> forgotten;
  std::shared_ptr<IBehaviourState>& behaviourState =
      view.behaviourState == nullptr ? forgotten : *view.behaviourState;
  auto* found = dynamic_cast<LuauBehaviourState*>(behaviourState.get());
  if (found == nullptr) {
    auto fresh = std::make_shared<LuauBehaviourState>();
    found = fresh.get();
    behaviourState = std::move(fresh);
  }
  try {
    refuseAfterAFailedSubscribedFunction(found->subscriptions);
    return resumeOrHold(*found, view);
  } catch (const engine::ScriptException& error) {
    stopOnFailure(*found, error);
    removeAfterFailure(view);
    return found->heldIntent.held();
  }
}

Intent LuauBehaviour::resumeOrHold(LuauBehaviourState& behaviourState,
                                   const BehaviourView& view) const {
  if (behaviourState.heldIntent.stopped()) {
    removeAfterFailure(view);
  }
  const std::optional<Intent> held = behaviourState.heldIntent.heldThisTick();
  if (held.has_value()) {
    return *held;
  }
  engine::ScriptMachine& machine = parts_.machine.get();
  if (!machine.hasTickBudget()) {
    return behaviourState.heldIntent.held();
  }
  ResumeInputs start{.parts = parts_,
                     .behaviourState = behaviourState,
                     .view = view,
                     .settingNames = settingNames_};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = pushResumeInputs, .context = &start});
  openStep(parts_, start);
  const engine::LuauResumeResult end = machine.resume(start.thread, 1);
  closeStep(parts_);
  const std::uint32_t waitTicks =
      ticksToWaitAfter(end, parts_.resumeContext.get());
  behaviourState.waiting = end == engine::LuauResumeResult::Yielded;
  ResumeOutputs result{.machine = machine.state(), .thread = start.thread};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = readResumeOutputs, .context = &result});
  return behaviourState.heldIntent.startHolding(result.intent, waitTicks);
}

void LuauBehaviour::stopOnFailure(LuauBehaviourState& behaviourState,
                                  const engine::ScriptException& error) const {
  closeStep(parts_);
  behaviourState.subscriptions.clear();
  behaviourState.thread.reset();
  behaviourState.waiting = false;
  behaviourState.heldIntent.stop();
  if (!failureReported_) {
    failureReported_ = true;
    parts_.reportFailure("the behaviour " + parts_.name +
                         " failed: " + error.what());
  }
}

void LuauBehaviour::removeAfterFailure(const BehaviourView& view) const {
  IEntityOrders* const orders = parts_.resumeContext.get().orders;
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
