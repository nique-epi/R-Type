#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundDescription.hpp"
#include "ShownBackgrounds.hpp"
#include "SkyLevel.hpp"

/** @brief The backgrounds the repository ships, as the sky shows them. */
class ShownBackgroundsFixture : public ::testing::Test {
 protected:
  /** @returns The backgrounds of the repository, the sky shown. */
  [[nodiscard]] rtype::client::ShownBackgrounds shippedSky() const;

  /** @returns The description the repository ships under @p name, that of
   * the sky for a name it lacks. */
  [[nodiscard]] const rtype::client::BackgroundDescription& described(
      std::string_view name) const;

 private:
  rtype::client::BackgroundCatalog shipped_;

 public:
  ShownBackgroundsFixture();
};

/** @returns A level in the biome of the Pit that began WHERE_THE_PIT_BEGAN
 * units in, @p unitsIn units ago, its scroll paused. */
[[nodiscard]] rtype::client::SkyLevel levelInThePit(float unitsIn);

/** @brief Moves @p backgrounds on by one second, a frame at a time. */
void passOneSecond(rtype::client::ShownBackgrounds& backgrounds);
