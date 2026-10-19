#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "MotionPreference.hpp"
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

/** @brief One bay of the waiting room: its player number, the color of its
 * ship, 0 to PLAYER_COLOR_COUNT - 1, and its pilot, none while it is open. */
struct HangarBay {
  int playerNumber{};
  int color{};
  std::optional<BayPilot> pilot;
};

/**
 * @brief Paints a bay, HANGAR_BAY_SIZE large.
 *
 * With a pilot: its number, YOU for the local player, a band of the ship
 * color, the ship enlarged BAY_SHIP_SCALE times, the nickname, and a cell that
 * says whether the pilot is ready, gold while the server does not show what
 * the local player asked. A ready pilot's ship plays PLAYER_SHIP_ANIMATION,
 * engines running; any other shows its first frame.
 *
 * Without one: its number, docking clamps around a row of beacons lit one
 * after the other, then OPEN BAY.
 *
 * With MotionPreference::Reduced, every ship shows its first frame and no
 * beacon lights.
 *
 * @param topLeft Top-left corner of the bay, in pixels of the interface
 * picture.
 * @param elapsed Time since the bays started showing: the time of the ships
 * and of the beacons.
 */
void paintHangarBay(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                    const HangarBay& bay, engine::Duration elapsed,
                    MotionPreference motion);

}  // namespace rtype::client
