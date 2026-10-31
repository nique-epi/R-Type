#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief The warning of a silent server in a game: shown once no message of
 * the server came for SILENCE_WARNING_DELAY, until the next one; never again
 * once stopped, the server going quiet on purpose at the end of a game.
 */
class SilenceWarning {
 public:
  /** @brief Starts the silence over: a message of the server arrived. */
  void hear();

  /** @brief Adds @p elapsed to the silence. */
  void advance(engine::Duration elapsed);

  /** @brief Never shows again. */
  void stop();

  /** @returns Whether the warning shows: SILENCE_WARNING_DELAY of silence or
   * more, and not stopped. */
  [[nodiscard]] bool isShown() const;

  /** @brief Paints the warning in the top-right corner of the playfield while
   * it shows: the bars of a weak signal, then WAITING FOR THE SERVER to their
   * left, outlined in ink. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  engine::Duration silence_{};
  bool stopped_ = false;
};

}  // namespace rtype::client
