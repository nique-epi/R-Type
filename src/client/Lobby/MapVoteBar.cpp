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

/** @returns Whether @p event presses (true) or releases (false) MAP_KEY;
 * nothing for any other event. */
std::optional<bool> mapKeyDownOf(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>();
      pressed != nullptr && pressed->scancode == MAP_KEY) {
    return true;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->scancode == MAP_KEY) {
    return false;
  }
  return std::nullopt;
}

}  // namespace

MapVoteBar::MapVoteBar(IMapBallot& ballot, const IWaitingRoom& room,
                       ISoundPlayer& sounds)
    : ballot_(&ballot), room_(&room), sounds_(&sounds) {}

bool MapVoteBar::handleEvent(const sf::Event& event) {
  const std::optional<bool> down = mapKeyDownOf(event);
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
  const std::uint8_t shown = shownMap();
  const std::optional<std::uint8_t> requested = ballot_->requestedMap();
  if (!requested.has_value() || *requested == network::NO_MAP_VOTE) {
    ballot_->voteFor(shown);
    return;
  }
  const std::size_t mapCount = room_->lobbyMaps().size();
  ballot_->voteFor(static_cast<std::uint8_t>((shown % mapCount) + 1));
}

bool MapVoteBar::isOpen() const { return !room_->lobbyMaps().empty(); }

std::string MapVoteBar::label() const {
  if (!isOpen()) {
    return MAP_BAR_CLOSED;
  }
  return MAP_BAR_PREFIX + room_->lobbyMaps()[shownMap() - 1];
}

std::uint8_t MapVoteBar::shownMap() const {
  const std::size_t mapCount = room_->lobbyMaps().size();
  const std::optional<std::uint8_t> requested = ballot_->requestedMap();
  if (requested.has_value() && *requested != network::NO_MAP_VOTE &&
      *requested <= mapCount) {
    return *requested;
  }
  return leadingMap(room_->lobbyPlayers(), mapCount);
}

}  // namespace rtype::client
