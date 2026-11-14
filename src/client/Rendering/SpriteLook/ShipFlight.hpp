#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include "Position.hpp"
#include "ShipFlightConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of the ship of a pilot: what the newest world state says
 * the pilot does, and the look the client draws from it: its flame, its
 * flight lines, its roll and its afterimages. EntityReplicator copies the
 * motion of each world state into it; ShipFlightSystem moves the look on at
 * each frame.
 */
struct ShipFlight {
  /** @brief The pilot pushes forward: it holds right, and not left. */
  bool forward{false};
  /** @brief The pilot holds up or down, not both. */
  bool sideways{false};
  /** @brief The server says the ship rolls. */
  bool rolling{false};
  /** @brief The server says the ship dashes. */
  bool dashing{false};
  /** @brief Whether the ship rolled at the frame before, so one roll of the
   * server starts one roll drawn. */
  bool rolledBefore{false};
  /** @brief The flame, 0 when out, 1 at full thrust. */
  float flame{};
  /** @brief How long the pilot has pushed forward without a break. */
  engine::Duration forwardTime{};
  /** @brief How far the roll drawn has turned, 0 to 1 over
   * game::ROLL_DURATION; nothing while none is drawn. */
  std::optional<float> rollShare;
  /** @brief Time since the ship got its ShipFlight: the flicker of its flame
   * and the scroll of its lines. */
  engine::Duration age{};
  /** @brief Where the ship was at each of its last frames, newest first, and
   * how many of them are known. */
  std::array<game::Position, AFTERIMAGE_TRAIL_LENGTH> trail{};
  std::size_t trailLength{};
};

}  // namespace rtype::client
