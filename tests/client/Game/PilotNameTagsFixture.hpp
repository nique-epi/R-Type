#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "PilotStatus.hpp"
#include "PlayerSettings.hpp"
#include "Position.hpp"

/** @returns A pilot of @p number with @p nickname, flying, its lives not
 * known yet. */
rtype::client::PilotStatus pilotNamed(std::uint8_t number,
                                      std::string nickname);

/** @brief The entities of a game, the ships of pilots among them, and the
 * settings of the player, at those of a first launch. */
class PilotNameTagsFixture : public ::testing::Test {
 protected:
  /** @returns A ship of @p pilotColor at @p place, drawn with SHIP_FRAME. */
  rtype::engine::Entity spawnShip(int pilotColor, rtype::game::Position place);
  /** @brief Moves @p ship to @p place. */
  void moveShip(rtype::engine::Entity ship, rtype::game::Position place);
  /** @brief Tags the ships of @p pilots with the settings of the fixture. */
  void tag(std::span<const rtype::client::PilotStatus> pilots);
  /** @returns The tag of @p ship, or nothing when it has none. */
  [[nodiscard]] std::optional<rtype::client::Caption> tagOf(
      rtype::engine::Entity ship);
  [[nodiscard]] rtype::client::PlayerSettings& settings();

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
  rtype::client::PlayerSettings settings_;
};
