#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include "IMapBallot.hpp"
#include "IMapPreviews.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What the card of the map is built from. Every reference must outlive
 * the card.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct MapVoteCardParts {
  /** @brief Carries the vote. */
  std::reference_wrapper<IMapBallot> ballot;
  /** @brief Lists the maps and the votes. */
  std::reference_wrapper<const IWaitingRoom> room;
  /** @brief Plays the click of MAP_KEY and of a click on the card. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Gives the preview of each map. */
  std::reference_wrapper<IMapPreviews> previews;
  /** @brief The colors of the pips of the pilots who vote. */
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief Where the card lies, in pixels of the interface. */
  sf::IntRect area;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
};

}  // namespace rtype::client
