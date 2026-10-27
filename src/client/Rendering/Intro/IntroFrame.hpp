#pragma once

#include <SFML/System/Vector2.hpp>
#include <functional>
#include "CinemaView.hpp"
#include "FlashPreference.hpp"
#include "IntroCast.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

/** @brief What every painter of the intro reads for one frame: the cast, the
 * time of the music and of the scene, the part of the world shown, the shake
 * of the camera in pixels of the canvas, how far left the debris drifted,
 * whether effects may flash, the palette pilots are painted in, and where the
 * title screen lays the box of its squadron, which the ships land in.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct IntroFrame {
  std::reference_wrapper<const IntroCast> cast;
  float real{};
  float scene{};
  CinemaView view;
  sf::Vector2i shake;
  float drift{};
  FlashPreference preference{};
  std::reference_wrapper<const PlayerPalette> palette;
  sf::Vector2i squadronBox;
};

}  // namespace rtype::client
