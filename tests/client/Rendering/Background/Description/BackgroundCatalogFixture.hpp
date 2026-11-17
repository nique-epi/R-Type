#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundDescription.hpp"
#include "TemporaryFolder.hpp"

/** @brief Two content folders below one temporary folder, the assets and a
 * resource, that the tests fill before reading their backgrounds. */
class BackgroundCatalogFixture : public ::testing::Test {
 protected:
  /** @brief Writes @p content to the file @p file below the temporary
   * folder. */
  void write(const std::filesystem::path& file, std::string_view content) const;

  /** @returns The backgrounds of the assets alone. */
  [[nodiscard]] rtype::client::BackgroundCatalog catalogOfTheAssets() const;

  /** @returns The backgrounds of the assets, then of the resource laid over
   * them. */
  [[nodiscard]] rtype::client::BackgroundCatalog catalogOfBothFolders() const;

  /** @returns The whole path of the file @p file of the temporary folder, as
   * the catalog names a file it finds outside the assets. */
  [[nodiscard]] std::string wholePathOf(std::string_view file) const;

 private:
  TemporaryFolder folder_;
};

/** @returns The name of the first layer of @p background, or nothing when
 * there is no background or no layer. */
[[nodiscard]] std::string firstLayerNameOf(
    const rtype::client::BackgroundDescription* background);

/** @returns The file the image of the first layer of @p background names, or
 * nothing when there is no background, no layer or no image. */
[[nodiscard]] std::string firstImageFileOf(
    const rtype::client::BackgroundDescription* background);

/** @returns The backgrounds of the assets the repository ships. */
[[nodiscard]] rtype::client::BackgroundCatalog shippedBackgrounds();
