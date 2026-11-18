#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include "EmissionGroupDescription.hpp"
#include "EmissionGroupMember.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The pace and the ceiling layers of particles share (see
 * EmissionGroupDescription): at each deadline, one of its members, drawn by
 * their weights, gives birth to a particle, and the next deadline is drawn.
 * While as many particles as the group allows live, a deadline that is due
 * waits, and the birth happens as soon as one of them ends.
 */
class EmissionGroup {
 public:
  /**
   * @param members The layers of the group, which must outlive it; a group
   * without members never gives birth.
   * @param seed Seed of every random draw of the group.
   */
  EmissionGroup(const EmissionGroupDescription& description,
                std::vector<EmissionGroupMember> members, std::uint32_t seed);

  /** @brief Moves the deadline on by @p elapsed, and gives birth when it is
   * due and the group has room. */
  void advance(engine::Duration elapsed);

  /** @returns How many particles of its members live. */
  [[nodiscard]] std::size_t aliveCount() const;

 private:
  EmissionGroupDescription description_;
  std::vector<EmissionGroupMember> members_;
  std::mt19937 random_;
  std::discrete_distribution<std::size_t> ranks_;
  engine::Duration untilNextBirth_;
};

}  // namespace rtype::client
