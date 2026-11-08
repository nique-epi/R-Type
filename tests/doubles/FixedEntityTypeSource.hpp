#pragma once

#include <cstdint>
#include <optional>
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "EntityTypeLook.hpp"
#include "IEntityTypeSource.hpp"
#include "ShipGun.hpp"

/**
 * @returns The appearance the double gives every type of side @p side: the
 * frames and the frame time of its row of the entity sheet, as the client drew
 * each side before the sheets, so a test reads a known look.
 */
[[nodiscard]] const rtype::client::EntityAppearance& appearanceOfSide(
    rtype::game::EntityType side);

/**
 * @brief The table of the five shipped sheets, numbered as
 * FixedEntityTypeSourceConstants.hpp lists them, each looking as
 * appearanceOfSide() gives its side.
 */
class FixedEntityTypeSource final : public rtype::client::IEntityTypeSource {
 public:
  [[nodiscard]] std::optional<rtype::client::EntityTypeLook> lookOf(
      std::uint8_t number) const override;

  [[nodiscard]] rtype::game::ShipGun shipGun() const override;
};
