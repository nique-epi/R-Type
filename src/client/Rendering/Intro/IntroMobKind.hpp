#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Which sprite of the entity sheet a Bydo of the intro shows. */
enum class IntroMobKind : std::uint8_t {
  Bydo,
  BonusBydo,
};

}  // namespace rtype::client
