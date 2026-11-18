#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "CountingEmitter.hpp"
#include "EmissionGroup.hpp"

/** @brief The group of the sky events over two counting emitters, the first
 * four times as likely as the second. */
class EmissionGroupFixture : public ::testing::Test {
 protected:
  /** @returns The group of the sky events, giving birth in likely() and in
   * rare(). */
  [[nodiscard]] rtype::client::EmissionGroup skyEvents();

  /** @returns The group of the sky events without any member. */
  [[nodiscard]] static rtype::client::EmissionGroup skyEventsWithoutMembers();

  [[nodiscard]] CountingEmitter& likely();
  [[nodiscard]] CountingEmitter& rare();

  /** @returns How many births both members were asked for. */
  [[nodiscard]] std::size_t births() const;

 private:
  CountingEmitter likely_;
  CountingEmitter rare_;
};
