#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include "GameResult.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenServices.hpp"
#include "ScrollingBackground.hpp"

namespace rtype::client {

/**
 * @brief What a screen is built from: the services the screens module got (the
 * sound among them), and what ClientScreens owns and every screen shares. Every
 * reference must outlive the screens.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ScreenContext {
  ScreenServices services;
  std::reference_wrapper<IScreenNavigator> navigator;
  std::reference_wrapper<ScrollingBackground> sky;
  std::reference_wrapper<GameResult> gameResult;
  std::reference_wrapper<std::optional<std::size_t>> previousTaunt;
};

}  // namespace rtype::client
