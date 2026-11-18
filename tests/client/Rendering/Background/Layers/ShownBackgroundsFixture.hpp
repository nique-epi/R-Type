#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundDescription.hpp"
#include "ShownBackgrounds.hpp"
#include "SkyLevel.hpp"
#include "TemporaryFolder.hpp"

/** @brief The backgrounds the repository ships, as the sky shows them. */
class ShownBackgroundsFixture : public ::testing::Test {
 protected:
  /** @returns The backgrounds of the repository, the sky shown. */
  [[nodiscard]] rtype::client::ShownBackgrounds shippedSky() const;

  /** @returns The description the repository ships under @p name, that of
   * the sky for a name it lacks. */
  [[nodiscard]] const rtype::client::BackgroundDescription& described(
      std::string_view name) const;

  /** @returns The backgrounds of the pack of a server, laid over the assets
   * of the repository: the one background whose file @p file holds
   * @p text. */
  [[nodiscard]] std::shared_ptr<const rtype::client::BackgroundCatalog> packOf(
      std::string_view file, std::string_view text) const;

 private:
  rtype::client::BackgroundCatalog shipped_;
  TemporaryFolder pack_;

 public:
  ShownBackgroundsFixture();
};

/** @returns A level in the biome of the Pit that began WHERE_THE_PIT_BEGAN
 * units in, @p unitsIn units ago, its scroll paused. */
[[nodiscard]] rtype::client::SkyLevel levelInThePit(float unitsIn);

/** @brief Moves @p backgrounds on by one second, a frame at a time. */
void passOneSecond(rtype::client::ShownBackgrounds& backgrounds);
