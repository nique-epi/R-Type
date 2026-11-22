#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "IMapBallot.hpp"
#include "IMapPreviews.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "Language.hpp"
#include "MapVoteCardParts.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief The card of the map in the waiting room, over the MAP bar beside
 * READY: the preview of the map it shows, its name, a pip for each pilot who
 * votes for it, and the keycap of MAP_KEY; a press votes for a map.
 *
 * It shows the map the player asked to vote for; before any vote of the
 * player, the map with the most votes in the room, the first of them on a
 * tie; while nobody voted, or once the player voted for a random map
 * (NO_MAP_VOTE), no map, but NO_MAP_CHOSEN_NAME. A press votes for the map
 * shown while the player has not voted, the first map while nobody did, then
 * for the next choice: the maps in their order, then a random one, then the
 * first map again; pressing back votes for the choice before. MAP_KEY presses
 * it once per hold, with the click of a bar, as the READY key does; a click on
 * the card presses it, or presses back left of the preview. With no map listed
 * (a server that plays one level) it is closed: the MAP bar under it says so,
 * and a press votes for nothing.
 */
class MapVoteCard {
 public:
  explicit MapVoteCard(MapVoteCardParts parts);

  /** @returns Whether @p event pressed or released MAP_KEY, or clicked the
   * open card; a press votes as press() does, once per hold, a click left of
   * the preview as pressBack() does. FocusLost forgets the key held. */
  bool handleEvent(const sf::Event& event);

  /** @brief Votes for the map shown when the player has not voted, the next
   * choice otherwise, a random map after the last; nothing while the card is
   * closed. */
  void press();

  /** @brief Votes for the choice before the one shown, a random map before the
   * first, the last map before a random one; nothing while the card is
   * closed. */
  void pressBack();

  /** @returns Whether the room lists maps to vote for. */
  [[nodiscard]] bool isOpen() const;

  /** @returns MAP_BAR_PREFIX and the name of the map shown, or
   * NO_MAP_CHOSEN_NAME; MAP_BAR_CLOSED while the card is closed. */
  [[nodiscard]] std::string label(Language language) const;

  /** @brief Paints the open card on @p canvas, its edge lit while
   * @p selected; nothing while it is closed. */
  void paint(IInterfaceCanvas& canvas, bool selected);

 private:
  /** @returns The name of the map shown, or NO_MAP_CHOSEN_NAME. */
  [[nodiscard]] std::string shownName(Language language) const;

  /** @returns The map shown, numbered from 1: the one voted for, or else the
   * one leading the vote; nothing while nobody voted, or once the player
   * voted for a random map. */
  [[nodiscard]] std::optional<std::uint8_t> shownMap() const;

  /** @returns The map the player asked to vote for, from 1; nothing before a
   * vote, for a vote taken back, or for a map the room does not list. */
  [[nodiscard]] std::optional<std::uint8_t> votedMap() const;

  /** @brief Votes for the choice @p step after the one shown, among the maps
   * and a random map (NO_MAP_VOTE, before the first), wrapping around: 1 for
   * press(), the count of the maps for pressBack(). */
  void voteAfter(std::size_t step);

  IMapBallot* ballot_;
  const IWaitingRoom* room_;
  ISoundPlayer* sounds_;
  IMapPreviews* previews_;
  std::reference_wrapper<const PlayerPalette> palette_;
  sf::IntRect area_;
  PlayfieldMapping playfieldPointAt_;
  bool keyHeld_ = false;
};

}  // namespace rtype::client
