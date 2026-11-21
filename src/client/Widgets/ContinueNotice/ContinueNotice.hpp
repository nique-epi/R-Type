#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief The notice of a team sent back to its checkpoint: CHECKPOINT and the
 * continues it has left, across the middle of the playfield, for
 * CONTINUE_NOTICE_TIME after the LevelStatus that told one was spent.
 *
 * A status tells a continue spent when it holds fewer continues than the
 * status before it: the first status a client gets, at the start of a level
 * or when it joins late, shows nothing. The LevelStatus are guaranteed and
 * ordered, so no older status comes after a newer one.
 */
class ContinueNotice {
 public:
  /** @brief Takes the continues a LevelStatus tells, received at @p now
   * since the start of the game. */
  void follow(std::uint8_t continuesLeft, engine::Duration now);
  /** @returns The continues the notice shows at @p now, nothing once its
   * time is over or before any was spent. */
  [[nodiscard]] std::optional<std::uint8_t> shownAt(engine::Duration now) const;
  /** @brief Paints the notice of @p continuesLeft: the band, its rules and
   * its line, outlined in ink. */
  static void paint(IInterfaceCanvas& canvas, std::uint8_t continuesLeft);

 private:
  std::optional<std::uint8_t> known_;
  std::optional<engine::Duration> spentAt_;
  std::uint8_t shown_{0};
};

/** @returns The line of the notice for @p continuesLeft: CHECKPOINT - 2
 * CONTINUES LEFT, CHECKPOINT - 1 CONTINUE LEFT, CHECKPOINT - NO CONTINUE
 * LEFT. */
[[nodiscard]] std::string continueNoticeText(std::uint8_t continuesLeft);

}  // namespace rtype::client
