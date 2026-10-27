#pragma once

#include <functional>
#include "GameResult.hpp"
#include "IEntityMessageSource.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISky.hpp"
#include "ISoundPlayer.hpp"

namespace rtype::client {

struct PlayerSettings;

}  // namespace rtype::client

namespace rtype::client {

/** @brief What a game screen is built from, but the pictures it owns. */
struct GameScreenParts {
  /** @brief Screens the game lives among, which must outlive it; the
   * options are pushed over the game when OPTIONS_KEY is released. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Settings of the player, which must outlive the screen: the keys
   * of the controls, and whether effects reduce their flashes, read at each
   * update. */
  std::reference_wrapper<const PlayerSettings> settings;
  /** @brief Where the actions held go to the server, which must outlive the
   * screen. */
  std::reference_wrapper<IPlayerInputLink> inputs;
  /** @brief Where the entity messages of the server come from, which must
   * outlive the screen. */
  std::reference_wrapper<IEntityMessageSource> entityMessages;
  /** @brief The session the game started in: its waiting room and the
   * number of this client, read when the screen is built. */
  std::reference_wrapper<const IServerConnection> connection;
  /** @brief The sky behind the screens, which must outlive the screen: its
   * stars speed up at the start of the game, and it shakes with the
   * camera. */
  std::reference_wrapper<ISky> sky;
  /** @brief Plays the sounds of what the server announces, which must
   * outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Where the game writes how it ended, once the server ends it,
   * which must outlive the screen. */
  std::reference_wrapper<GameResult> result;
};

}  // namespace rtype::client
