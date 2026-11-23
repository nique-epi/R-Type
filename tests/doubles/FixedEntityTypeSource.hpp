#pragma once

#include <cstdint>
#include <optional>
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "EntityTypeLook.hpp"
#include "IEntityTypeSource.hpp"
#include "ShipArsenal.hpp"

/**
 * @returns The appearance the double gives every type of side @p side: the
 * frames and the frame time of its row of the entity sheet, as the client drew
 * each side before the sheets, so a test reads a known look.
 */
[[nodiscard]] const rtype::client::EntityAppearance& appearanceOfSide(
    rtype::game::EntityType side);

/**
 * @brief The table of the sheets FixedEntityTypeSourceConstants.hpp lists,
 * numbered as it numbers them, each looking as appearanceOfSide() gives its
 * side; the enemy of two boxes lists TWO_BOX_ENEMY_PARTS, and the hollow
 * enemy has a collision mask of HOLLOW_ENEMY_SIDE pixels a side, its
 * HOLLOW_ENEMY_DRAWN_ROWS top rows alone drawn. Its ship holds the gun of
 * SHIP_GUN_INTERVAL_TICKS and the two weapons FAN_WEAPON and ROWS_WEAPON name.
 */
class FixedEntityTypeSource final : public rtype::client::IEntityTypeSource {
 public:
  [[nodiscard]] std::optional<rtype::client::EntityTypeLook> lookOf(
      std::uint8_t number) const override;

  [[nodiscard]] const rtype::game::ShipArsenal& shipArsenal() const override;
};
