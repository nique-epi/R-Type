#include "LuauLevelRun.hpp"
#include <functional>
#include <optional>
#include <string>
#include "EngineException.hpp"
#include "LevelCheckpoint.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelRunParts.hpp"
#include "LuauLevelRunState.hpp"
#include "LuauLevelStep.hpp"
#include "LuauLevelWait.hpp"
#include "LuauLevels.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeResult.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief What pushing the function of a stage on its thread needs: the
 * thread, created when there is none, the machine, and the reference of the
 * function, nothing for a stage resumed in its wait. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct StageStart {
  std::reference_wrapper<std::optional<engine::LuauThread>> thread;
  lua_State* machine{nullptr};
  std::optional<int> function;
  lua_State* resumed{nullptr};
};

/** @brief Creates the thread of a stage when it has none, and pushes on it
 * the function of a stage that starts. */
void pushStage(void* context) {
  auto& start = *static_cast<StageStart*>(context);
  std::optional<engine::LuauThread>& thread = start.thread.get();
  if (!thread.has_value()) {
    thread.emplace(start.machine);
  }
  start.resumed = thread->state();
  if (start.function.has_value()) {
    lua_getref(start.machine, *start.function);
    lua_xmove(start.machine, start.resumed, 1);
  }
}

}  // namespace

LuauLevelRun::LuauLevelRun(LuauLevelRunParts parts) : parts_(parts) {
  parts_.levels.get().attach(parts_.progress.get(), parts_.components.get(),
                             parts_.entry.get());
}

LuauLevelRun::~LuauLevelRun() {
  parts_.levels.get().detach(parts_.progress.get());
}

void LuauLevelRun::step() {
  if (state_ != LuauLevelRunState::Running || !waitIsOver() ||
      !parts_.machine.get().hasTickBudget()) {
    return;
  }
  try {
    resumeStage();
  } catch (const engine::ScriptException& error) {
    fail(error.what());
  }
}

bool LuauLevelRun::restartFromCheckpoint() {
  LevelProgress& progress = parts_.progress.get();
  if (state_ == LuauLevelRunState::Failed || !progress.checkpoint.has_value()) {
    return false;
  }
  const LevelCheckpoint& checkpoint = *progress.checkpoint;
  thread_.reset();
  wait_.reset();
  stage_ = checkpoint.stage;
  state_ = LuauLevelRunState::Running;
  progress.scroll.returnTo(checkpoint.travelled);
  progress.scroll.resume();
  progress.biome = checkpoint.biome;
  return true;
}

LuauLevelRunState LuauLevelRun::state() const { return state_; }

const std::string& LuauLevelRun::failure() const { return failure_; }

bool LuauLevelRun::waitIsOver() {
  if (!wait_.has_value()) {
    return true;
  }
  if (wait_->ticks > 1) {
    --wait_->ticks;
    return false;
  }
  if (parts_.progress.get().scroll.position() < wait_->untilScrollPosition) {
    return false;
  }
  wait_.reset();
  return true;
}

void LuauLevelRun::resumeStage() {
  engine::ScriptMachine& machine = parts_.machine.get();
  StageStart start{
      .thread = thread_,
      .machine = machine.state(),
      .function = thread_.has_value()
                      ? std::nullopt
                      : std::optional{parts_.entry.get().stages[stage_]}};
  engine::runProtected(
      machine.state(),
      engine::LuauProtectedWork{.run = pushStage, .context = &start});
  LuauLevelStep& step = parts_.levels.get().step();
  step.resumedThread = start.resumed;
  step.wait.reset();
  const engine::LuauResumeResult end = machine.resume(start.resumed, 0);
  step.resumedThread = nullptr;
  if (end == engine::LuauResumeResult::Returned) {
    thread_.reset();
    endStage();
    return;
  }
  if (!step.wait.has_value()) {
    throw engine::ScriptException(LEVEL_YIELD_MESSAGE);
  }
  wait_ = step.wait;
}

void LuauLevelRun::endStage() {
  ++stage_;
  if (stage_ >= parts_.entry.get().stages.size()) {
    state_ = LuauLevelRunState::Cleared;
    return;
  }
  LevelProgress& progress = parts_.progress.get();
  progress.checkpoint =
      LevelCheckpoint{.stage = stage_,
                      .travelled = progress.scroll.travelled(),
                      .biome = progress.biome};
}

void LuauLevelRun::fail(const std::string& reason) {
  parts_.levels.get().step().resumedThread = nullptr;
  thread_.reset();
  wait_.reset();
  state_ = LuauLevelRunState::Failed;
  failure_ = reason;
}

}  // namespace rtype::game
