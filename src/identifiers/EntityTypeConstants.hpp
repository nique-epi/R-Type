#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::identifiers {

/**
 * @brief The type number no entity type has: a type is numbered from 1 by its
 * place in the table of ConnectionAccepted, which is the order of the sheets,
 * so 0 never names one.
 */
constexpr std::uint8_t NO_ENTITY_TYPE = 0;

/**
 * @brief The most entity types a program loads: a type is numbered from 1 on
 * one byte, and NO_ENTITY_TYPE takes 0, so 255 is the last number.
 */
constexpr std::size_t MAXIMUM_ENTITY_TYPES = 255;

}  // namespace rtype::identifiers
