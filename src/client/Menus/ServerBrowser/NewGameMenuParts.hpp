#pragma once

#include <functional>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "ServerChoice.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/**
 * @brief What the new game dialog is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct NewGameMenuParts {
  /** @brief Screens around the dialog, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the dialog, and must outlive it. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Where CREATE writes the server and the game of the attempt; it
   * must outlive the dialog. */
  std::reference_wrapper<ServerChoice> choice;
  /** @brief The server the new game opens on, whose games the map listed. */
  network::Endpoint server;
  /** @brief The difficulties of the server, as the master lists them. */
  std::vector<std::string> difficulties;
  /** @brief The text size the dialog is laid out at. */
  TextMetrics metrics{};
};

}  // namespace rtype::client
