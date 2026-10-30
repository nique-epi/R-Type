#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <cstddef>
#include <functional>
#include <optional>
#include "ClientScreensParts.hpp"
#include "GameResult.hpp"
#include "IScreenNavigator.hpp"
#include "ScrollingBackground.hpp"
#include "ServerChoice.hpp"

namespace rtype::client {

/**
 * @brief What the screens are built from: the services the screens module
 * got (the sound among them), and what ClientScreens owns and every screen
 * shares. Every reference must outlive the catalog.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ScreenCatalogParts {
  ClientScreensParts services;
  std::reference_wrapper<IScreenNavigator> navigator;
  std::reference_wrapper<ScrollingBackground> sky;
  std::reference_wrapper<GameResult> gameResult;
  std::reference_wrapper<std::optional<std::size_t>> previousTaunt;
  std::reference_wrapper<ServerChoice> serverChoice;
  std::reference_wrapper<sf::Texture> lostPicture;
};

}  // namespace rtype::client
