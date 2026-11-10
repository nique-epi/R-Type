#pragma once

#include <gtest/gtest.h>
#include <Velopack.hpp>
#include <cstdint>
#include <memory>
#include <string_view>
#include "FakeReleaseFiles.hpp"
#include "ReleaseFeedSource.hpp"
#include "TemporaryFolder.hpp"

/** @brief A ReleaseFeedSource over release files the test serves. */
class ReleaseFeedSourceFixture : public ::testing::Test {
 protected:
  ReleaseFeedSourceFixture();

  /** @returns A package of release PACKAGE_VERSION named @p fileName. */
  [[nodiscard]] static Velopack::VelopackAsset packageNamed(
      std::string_view fileName);
  /** @brief Saves @p package to the folder of the test, keeping the last share
   * reported in lastProgress(). @returns What the source answered. */
  [[nodiscard]] bool save(const Velopack::VelopackAsset& package);

  [[nodiscard]] rtype::client::ReleaseFeedSource& source();
  [[nodiscard]] FakeReleaseFiles& files();
  [[nodiscard]] std::int16_t lastProgress() const;

 private:
  FakeReleaseFiles* files_;
  std::unique_ptr<rtype::client::ReleaseFeedSource> source_;
  std::int16_t lastProgress_{};
  TemporaryFolder folder_;
};
