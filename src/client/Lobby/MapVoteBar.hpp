#pragma once

#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include "IMapBallot.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The MAP bar of the waiting room, beside READY: it names the map it
 * shows, and a press votes for a map.
 *
 * It shows the map the player asked to vote for; before any vote, the map
 * with the most votes in the room, the first of them on a tie, the first map
 * without votes. A press votes for the map shown while the player has not
 * voted, then for the next one, wrapping around. MAP_KEY presses it once per
 * hold, with the click of a bar, as the READY key does. With no map listed (a
 * server that plays one level) it is closed: it says so, and a press votes for
 * nothing.
 */
class MapVoteBar {
 public:
  /** @param ballot Carries the vote, @param room lists the maps and the
   * votes, @param sounds plays the click of MAP_KEY; all must outlive the
   * bar. */
  MapVoteBar(IMapBallot& ballot, const IWaitingRoom& room,
             ISoundPlayer& sounds);

  /** @returns Whether @p event pressed or released MAP_KEY; a press votes
   * as press() does, once per hold. FocusLost forgets the key held. */
  bool handleEvent(const sf::Event& event);

  /** @brief Votes for the map shown when the player has not voted, the next
   * one otherwise; nothing while the bar is closed. */
  void press();

  /** @returns Whether the room lists maps to vote for. */
  [[nodiscard]] bool isOpen() const;

  /** @returns MAP_BAR_PREFIX and the name of the map shown; MAP_BAR_CLOSED
   * while the bar is closed. */
  [[nodiscard]] std::string label() const;

 private:
  /** @returns The map shown, numbered from 1, the bar open: the one voted
   * for, or else the one leading the vote. */
  [[nodiscard]] std::uint8_t shownMap() const;

  /** @returns The map the player asked to vote for, from 1; nothing before a
   * vote, for a vote taken back, or for a map the room does not list. */
  [[nodiscard]] std::optional<std::uint8_t> votedMap() const;

  IMapBallot* ballot_;
  const IWaitingRoom* room_;
  ISoundPlayer* sounds_;
  bool keyHeld_ = false;
};

}  // namespace rtype::client
