#pragma once

#include <optional>
#include "IMusicPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The music of a client without sound: never plays, so its position
 * is never known and whoever follows it, as the intro does, follows the time
 * of the frames instead. */
class NullMusicPlayer final : public IMusicPlayer {
 public:
  void start() override;

  void stop() override;

  /** @returns Nothing: this music never plays. */
  [[nodiscard]] std::optional<engine::Duration> playingOffset() const override;
};

}  // namespace rtype::client
