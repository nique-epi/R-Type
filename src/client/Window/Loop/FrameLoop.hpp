#pragma once

#include "IClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {
class Application;
}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief The frame loop of the client: main() runs it on its thread, where
 * SFML reads the window events, once every module started. Each pass runs the
 * stages of the Application with the real time elapsed since the previous
 * pass, until a module stops it.
 *
 * The window module provides it to the modules installed after it: the
 * window stops it once it is closed, the screens once no screen is left. A
 * stop asked for during a pass ends the loop once that pass is over, so the
 * frame in progress is still drawn and shown.
 */
class FrameLoop {
 public:
  /** @param clock Measures the time between two passes; it must outlive the
   * loop. */
  explicit FrameLoop(const engine::IClock& clock);

  /**
   * @brief Runs the stages of @p application, pass after pass, until stop();
   * the first pass moves by the time since the call. Returns at once when
   * stop() was called before.
   * @throws What a work of a stage throws.
   */
  void run(const engine::Application& application);

  /** @brief Ends run() once the pass in progress is over. */
  void stop();

 private:
  /** @returns The time since the previous pass, from which the next one is
   * measured. */
  [[nodiscard]] engine::Duration measureFrame();

  const engine::IClock* clock_;
  engine::Duration previousFrameTime_{};
  bool stopped_{false};
};

}  // namespace rtype::client
