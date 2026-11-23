#pragma once

#include <optional>
#include <string_view>
#include "EntitySheet.hpp"
#include "Gun.hpp"

namespace rtype::server {

/**
 * @returns The gun a ship of @p ship holding @p held fires once it picks up a
 * crystal of the weapon @p weapon: the next level of that weapon when it
 * already holds it, never past its last, its first level otherwise, its count
 * to the next shot kept but never above the new interval. Nothing when the
 * sheet holds no weapon of that name.
 */
[[nodiscard]] std::optional<game::Gun> gunAfterPickup(
    const game::Gun& held, const game::EntitySheet& ship,
    std::string_view weapon);

}  // namespace rtype::server
