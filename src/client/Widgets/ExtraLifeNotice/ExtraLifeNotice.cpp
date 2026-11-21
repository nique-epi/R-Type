#include "ExtraLifeNotice.hpp"
#include <cstddef>
#include <optional>
#include "ExtraLifeNoticeConstants.hpp"
#include "ExtraLifeRises.hpp"
#include "GameEvent.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "PilotColor.hpp"
#include "StatusBand.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The bay of the player @p playerNumber, nothing for a number that
 * has none. */
std::optional<std::size_t> bayOfPlayer(int playerNumber) {
  const int bay = pilotColorOfPlayerNumber(playerNumber);
  if (!isPilotColor(bay)) {
    return std::nullopt;
  }
  return static_cast<std::size_t>(bay);
}

/** @returns How far a notice started @p sinceWon ago has risen, in rows. */
int riseAfter(engine::Duration sinceWon) {
  return static_cast<int>(EXTRA_LIFE_NOTICE_RISE * sinceWon /
                          EXTRA_LIFE_NOTICE_TIME);
}

}  // namespace

void ExtraLifeNotice::apply(const network::GameEvent& event,
                            engine::Duration now) {
  if (event.kind != network::GameEventKind::ExtraLifeWon) {
    return;
  }
  if (const std::optional<std::size_t> bay = bayOfPlayer(event.playerNumber)) {
    wonAt_.at(*bay) = now;
  }
}

ExtraLifeRises ExtraLifeNotice::risesAt(engine::Duration now) const {
  ExtraLifeRises rises{};
  for (std::size_t bay = 0; bay < wonAt_.size(); ++bay) {
    const std::optional<engine::Duration>& wonAt = wonAt_.at(bay);
    if (wonAt.has_value() && now - *wonAt < EXTRA_LIFE_NOTICE_TIME) {
      rises.at(bay) = riseAfter(now - *wonAt);
    }
  }
  return rises;
}

void ExtraLifeNotice::paint(IInterfaceCanvas& canvas,
                            const ExtraLifeRises& rises) {
  for (std::size_t bay = 0; bay < rises.size(); ++bay) {
    const std::optional<int>& rise = rises.at(bay);
    if (!rise.has_value()) {
      continue;
    }
    const int top = STATUS_BAND_AREA.position.y - TEXT_LINE_HEIGHT -
                    EXTRA_LIFE_NOTICE_GAP - *rise;
    canvas.write(
        EXTRA_LIFE_NOTICE_TEXT,
        {statusBayLeft(static_cast<int>(bay)) + STATUS_LIVES_TOP_LEFT.x, top},
        TextStyle{.font = TextFont::Regular,
                  .color = HUD_CYAN,
                  .outlined = true,
                  .alignment = TextAlignment::Left});
  }
}

}  // namespace rtype::client
