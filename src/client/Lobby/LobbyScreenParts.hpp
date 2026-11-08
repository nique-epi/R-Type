#pragma once

#include <functional>
#include <optional>
#include "GameRules.hpp"
#include "IChatChannel.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "IShipLookExchange.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "KeyBindings.hpp"
#include "KeyNames.hpp"
#include "LobbyPlace.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ShipLook.hpp"

namespace rtype::client {

/**
 * @brief What a waiting room is built from, but the canvas it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LobbyScreenParts {
  /** @brief Screens the waiting room lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu and of the ready key, which must
   * outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Session the waiting room leaves, which must outlive it: its
   * status gives the number of this client. */
  std::reference_wrapper<IServerConnection> connection;
  /** @brief The room the screen reads and drives, which must outlive it. */
  std::reference_wrapper<IWaitingRoom> waitingRoom;
  /** @brief Where the screen sends the look of the player and reads the look
   * of every pilot, which must outlive it. */
  std::reference_wrapper<IShipLookExchange> shipLooks;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief The keys the player chose, read when the screen is built: READY
   * shows the key bound to BoundAction::Ready, and the help names them all;
   * they must outlive the screen. */
  std::reference_wrapper<const KeyBindings> keys;
  /** @brief Gives the name a keycap shows for a key. */
  KeyNaming keyName;
  /** @brief The palette the ships and their bands are painted in: a
   * constant, so it outlives the screen. */
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief What the ship of the local player wears, read from the settings
   * at each frame, since the hangar pushed over the room changes it; it must
   * outlive the screen. */
  std::reference_wrapper<const identifiers::ShipLook> look;
  /** @brief Whether the ships and the beacons may move. */
  MotionPreference motion{};
  /** @brief Where BACK leads: the connection screen below, or the title. */
  LobbyPlace place{};
  /** @brief The chat of the session, which must outlive the screen. */
  std::reference_wrapper<IChatChannel> chat;
  /** @brief The settings of the player, which must outlive the screen: the
   * chat reads its key and its look there. */
  std::reference_wrapper<const PlayerSettings> settings;
  /** @brief The rules of the game the player waits for, written on the row
   * of the heading; nothing when the client does not know them. */
  std::optional<network::GameRules> rules = std::nullopt;
};

}  // namespace rtype::client
