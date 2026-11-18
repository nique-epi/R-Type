#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundDescription.hpp"
#include "LayerSources.hpp"
#include "LayeredBackground.hpp"

/** @returns Sources that find the files of the assets folder of the
 * repository, with the seed of the tests. */
[[nodiscard]] rtype::client::LayerSources shippedSources();

/** @brief The backgrounds the tests move: A_SMALL_SKY, and those the
 * repository ships, whose descriptions outlive what is built from them. */
class LayeredBackgroundFixture : public ::testing::Test {
 protected:
  /** @returns A_SMALL_SKY, built. */
  [[nodiscard]] rtype::client::LayeredBackground smallSky() const;

  /** @returns The background the repository ships under @p name, built. */
  [[nodiscard]] rtype::client::LayeredBackground shipped(
      std::string_view name) const;

  /** @returns The description of A_SMALL_SKY. */
  [[nodiscard]] const rtype::client::BackgroundDescription&
  smallSkyDescription() const;

  /** @returns The description of the Pit the repository ships. */
  [[nodiscard]] const rtype::client::BackgroundDescription& pitDescription()
      const;

 private:
  rtype::client::BackgroundDescription sky_;
  rtype::client::BackgroundCatalog shipped_;

 public:
  LayeredBackgroundFixture();
};

/** @returns Where the layer @p layer of @p background lies: the column it
 * is painted on, and how far of it it lies. */
[[nodiscard]] float whereLies(
    const rtype::client::LayeredBackground& background, std::size_t layer);

/** @returns How many pixels and frames the layers of @p background from
 * @p first on ask a canvas to paint. */
[[nodiscard]] std::size_t paintedFrom(
    const rtype::client::LayeredBackground& background, std::size_t first);
