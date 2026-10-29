#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include "ClientScreensParts.hpp"
#include "GameResult.hpp"
#include "IScreenNavigator.hpp"
#include "MusicPlayer.hpp"
#include "ScrollingBackground.hpp"
#include "SoundPlayer.hpp"
#include "Soundtrack.hpp"

namespace rtype::client {

/**
 * @brief What the screens are built from: the services the screens module
 * got, and what ClientScreens owns and every screen shares. Every reference
 * must outlive the catalog.
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
  std::reference_wrapper<SoundPlayer> sounds;
  std::reference_wrapper<MusicPlayer> theme;
  std::reference_wrapper<Soundtrack> soundtrack;
  std::reference_wrapper<GameResult> gameResult;
  std::reference_wrapper<std::optional<std::size_t>> previousTaunt;
};

}  // namespace rtype::client
