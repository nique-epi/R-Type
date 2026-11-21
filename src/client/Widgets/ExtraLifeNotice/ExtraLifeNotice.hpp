#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include "EntitySpriteConstants.hpp"
#include "ExtraLifeRises.hpp"
#include "GameEvent.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief The notices of the extra lives of a game: 1UP over the bay of each
 * pilot who won one, for EXTRA_LIFE_NOTICE_TIME after the GameEvent that
 * told it. A second life won meanwhile starts the notice of that bay over.
 */
class ExtraLifeNotice {
 public:
  /** @brief Takes @p event, received at @p now since the start of the game:
   * an ExtraLifeWon starts the notice of the bay of its player; any other
   * kind, or a player without a bay, changes nothing. */
  void apply(const network::GameEvent& event, engine::Duration now);
  /** @returns How far each notice has risen at @p now. */
  [[nodiscard]] ExtraLifeRises risesAt(engine::Duration now) const;
  /** @brief Paints each notice of @p rises over its bay, in HUD_CYAN,
   * outlined in ink. */
  static void paint(IInterfaceCanvas& canvas, const ExtraLifeRises& rises);

 private:
  std::array<std::optional<engine::Duration>,
             static_cast<std::size_t>(PLAYER_COLOR_COUNT)>
      wonAt_{};
};

}  // namespace rtype::client
