#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include "LuauLevelRunParts.hpp"
#include "LuauLevelRunState.hpp"
#include "LuauLevelWait.hpp"
#include "LuauThread.hpp"

namespace rtype::game {

/**
 * @brief The run of one level in one world: its stages, each a function of a
 * script run in a Luau thread, one after the other.
 *
 * step() is called once a tick, before the level scrolls: it starts the
 * stage, or resumes it once its wait is over, which costs a step of the
 * machine; a stage that waits costs nothing. When a stage returns, the next
 * one starts at the following step, and the place it starts from is kept as
 * the checkpoint of the level: the stage, how far the level had scrolled and
 * its biome. Once the last stage returned, the run is Cleared.
 * restartFromCheckpoint() puts the level back at that checkpoint and runs its
 * stage again from its first line; the first stage starts at no checkpoint.
 *
 * A stage that fails (an error of the script, its memory or its time budget)
 * stops the run for good: it is Failed, and failure() says why. Once the
 * budget of a tick is spent, the stage due steps at the next tick.
 *
 * It makes its progress the level the scripts reach for as long as it lives.
 * Neither copied nor moved; destroy it while no script runs and while the
 * machine is alive.
 */
class LuauLevelRun {
 public:
  explicit LuauLevelRun(LuauLevelRunParts parts);
  LuauLevelRun(const LuauLevelRun&) = delete;
  LuauLevelRun& operator=(const LuauLevelRun&) = delete;
  LuauLevelRun(LuauLevelRun&&) = delete;
  LuauLevelRun& operator=(LuauLevelRun&&) = delete;
  ~LuauLevelRun();

  /** @brief Lets one tick pass: starts or resumes the stage when it is
   * due. */
  void step();

  /**
   * @brief Puts the scroll and the biome back as they were at the last
   * checkpoint, the scroll moving, and has the stage that starts there run
   * again from its first line at the next step().
   * @returns Whether the level had reached a checkpoint and has not failed;
   * nothing changes otherwise.
   */
  bool restartFromCheckpoint();

  [[nodiscard]] LuauLevelRunState state() const;

  /** @returns Why the run failed; empty unless it is Failed. */
  [[nodiscard]] const std::string& failure() const;

 private:
  /** @returns Whether the stage may go on, after counting a tick of its
   * wait. */
  [[nodiscard]] bool waitIsOver();
  void resumeStage();
  void endStage();
  void fail(const std::string& reason);

  LuauLevelRunParts parts_;
  /** The thread of the stage that stopped in a wait, to be resumed there;
   * empty while no stage waits: the next step starts the stage due in a new
   * thread. */
  std::optional<engine::LuauThread> thread_;
  std::optional<LuauLevelWait> wait_;
  std::size_t stage_{0};
  LuauLevelRunState state_{LuauLevelRunState::Running};
  std::string failure_;
};

}  // namespace rtype::game
