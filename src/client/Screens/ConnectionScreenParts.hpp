#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"

namespace rtype::client {

/** @brief What a connection screen is built from, but the picture it owns. */
struct ConnectionScreenParts {
  /** @brief Screens the connection screen lives among, which must outlive
   * it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Connection the attempts go through, which must outlive the
   * screen. */
  std::reference_wrapper<IServerConnection> connection;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Address, port and nickname the fields start with; the screen
   * replaces them with those of each attempt it starts. It must outlive the
   * screen. */
  std::reference_wrapper<ServerChoice> choice;
};

}  // namespace rtype::client
