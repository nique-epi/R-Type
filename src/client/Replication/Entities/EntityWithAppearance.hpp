#pragma once

#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"

namespace rtype::client {

/**
 * @returns The entity whose network id is @p networkId and that has an
 * AppearanceReference to the appearance of its sheet, or nothing: an entity
 * the client does not have, or one drawn without a sheet, which a message
 * naming something of its sheet leaves as it is.
 */
[[nodiscard]] std::optional<engine::Entity> entityWithAppearance(
    engine::ComponentRegistry& components, std::uint32_t networkId);

}  // namespace rtype::client
