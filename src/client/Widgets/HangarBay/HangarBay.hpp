#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <functional>
#include <optional>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Whether the pilot of a bay is ready, as the server shows it, or what
 * the local player asked while the server does not show it yet. */
enum class BayReadiness : std::uint8_t {
  NotReady,
  Ready,
  ReadyAsked,
  NotReadyAsked,
};

/** @brief The pilot who took a bay. */
struct BayPilot {
  std::string_view nickname;
  BayReadiness readiness = BayReadiness::NotReady;
  bool isLocalPlayer = false;
};

/**
 * @brief One bay of the waiting room: where it lies, its player number, the
 * color of its ship, 0 to PLAYER_COLOR_COUNT - 1, the palette the player
 * chose, and its pilot, none while it is open.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HangarBay {
  /** @brief Top-left corner of the bay, in pixels of the interface picture. */
  sf::Vector2i topLeft;
  int playerNumber{};
  int color{};
  std::reference_wrapper<const PlayerPalette> palette;
  std::optional<BayPilot> pilot;
};

/** @brief How far the animations of the bays have run, and whether they may
 * move at all. */
struct BayMotion {
  /** @brief Time since the bays started showing: the time of the ships and of
   * the beacons. */
  engine::Duration elapsed{};
  /** @brief With MotionPreference::Reduced, every ship shows its first frame
   * and no beacon lights. */
  MotionPreference preference{};
};

/**
 * @brief Paints a bay, HANGAR_BAY_SIZE large.
 *
 * With a pilot: its number, YOU for the local player, a band of the ship
 * color, the ship enlarged BAY_SHIP_SCALE times, the nickname, and a cell that
 * says whether the pilot is ready, gold and ending in three dots while the
 * server does not show what the local player asked, so the wait is never told
 * by its color alone. A ready pilot's ship plays PLAYER_SHIP_ANIMATION,
 * engines running; any other shows its first frame.
 *
 * Without one: its number, docking clamps around a row of beacons lit one
 * after the other, then OPEN BAY.
 */
void paintHangarBay(IInterfaceCanvas& canvas, const HangarBay& bay,
                    BayMotion motion);

}  // namespace rtype::client
