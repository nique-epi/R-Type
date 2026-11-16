#pragma once

#include <cstdint>
#include <random>
#include "IRouletteDraw.hpp"

namespace rtype::network {

/**
 * @brief Draws each number from the random source of the system, with no
 * seeded engine in between, so a player who saw past numbers learns nothing
 * of the next one.
 *
 * std::random_device reads arc4random or getentropy with libc++, getentropy,
 * arc4random or /dev/urandom with libstdc++, and rand_s with the standard
 * library of MSVC; std::uniform_int_distribution brings its bits to 0..36 by
 * rejection in all three (Lemire's method in libstdc++ and MSVC), never by a
 * bare modulo, so every number is as likely as another.
 */
class SystemRouletteDraw final : public IRouletteDraw {
 public:
  [[nodiscard]] std::uint8_t drawNumber() override;

 private:
  std::random_device random_;
};

}  // namespace rtype::network
