#include "MapVoteBar.hpp"
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include "IMapBallot.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "KeyDown.hpp"
#include "LobbyScreenConstants.hpp"
#include "LobbyState.hpp"
#include "ProtocolConstants.hpp"
#include "SoundConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The map with the most votes of @p players among @p mapCount, from
 * 1, the first of them on a tie: map 1 when nobody voted. */
std::uint8_t leadingMap(std::span<const network::LobbyPlayer> players,
                        std::size_t mapCount) {
  std::uint8_t leading = 1;
  std::ptrdiff_t mostVotes = 0;
  for (std::size_t map = 1; map <= mapCount; ++map) {
    const auto number = static_cast<std::uint8_t>(map);
    const std::ptrdiff_t votes =
        std::ranges::count(players, number, &network::LobbyPlayer::mapVote);
    if (votes > mostVotes) {
      mostVotes = votes;
      leading = number;
    }
  }
  return leading;
}

}  // namespace

MapVoteBar::MapVoteBar(IMapBallot& ballot, const IWaitingRoom& room,
                       ISoundPlayer& sounds)
    : ballot_(&ballot), room_(&room), sounds_(&sounds) {}

bool MapVoteBar::handleEvent(const sf::Event& event) {
  const std::optional<bool> down = keyDownOf(event, MAP_KEY);
  if (!down.has_value()) {
    keyHeld_ = keyHeld_ && !event.is<sf::Event::FocusLost>();
    return false;
  }
  if (*down && !keyHeld_) {
    sounds_->play(CLICK_SOUND_ID);
    press();
  }
  keyHeld_ = *down;
  return true;
}

void MapVoteBar::press() {
  if (!isOpen()) {
    return;
  }
  const std::optional<std::uint8_t> voted = votedMap();
  const std::size_t mapCount = room_->lobbyMaps().size();
  ballot_->voteFor(voted.has_value()
                       ? static_cast<std::uint8_t>((*voted % mapCount) + 1)
                       : shownMap());
}

bool MapVoteBar::isOpen() const { return !room_->lobbyMaps().empty(); }

std::string MapVoteBar::label() const {
  if (!isOpen()) {
    return MAP_BAR_CLOSED;
  }
  return MAP_BAR_PREFIX + room_->lobbyMaps()[shownMap() - 1];
}

std::uint8_t MapVoteBar::shownMap() const {
  return votedMap().value_or(
      leadingMap(room_->lobbyPlayers(), room_->lobbyMaps().size()));
}

std::optional<std::uint8_t> MapVoteBar::votedMap() const {
  const std::optional<std::uint8_t> requested = ballot_->requestedMap();
  if (!requested.has_value() || *requested == network::NO_MAP_VOTE ||
      *requested > room_->lobbyMaps().size()) {
    return std::nullopt;
  }
  return requested;
}

}  // namespace rtype::client
