#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"

namespace rtype::client {

/** @brief Loads an asset list one file per call, after releasing the loaded
 * assets the list does not name; an asset already loaded is neither read again
 * nor counted. */
class AssetLoadingStep {
 public:
  /** @brief Releases the loaded assets the list does not name, except the
   * permanent ones, so whatever still uses them must already be destroyed. */
  AssetLoadingStep(AssetLibrary& library, const AssetList& assets);

  /** @brief Loads the next asset, then throws AssetLoadingException after the
   * last one if any asset could not be loaded. */
  void loadNext();

  [[nodiscard]] bool isFinished() const;
  [[nodiscard]] std::size_t processedCount() const;
  [[nodiscard]] std::size_t assetCount() const;

 private:
  struct PendingAsset {
    AssetKind kind;
    std::string assetId;
  };

  void record(const PendingAsset& asset, AssetLoadResult result);

  AssetLibrary* library_;
  std::vector<PendingAsset> pending_;
  std::size_t processedCount_{0};
  AssetProblems problems_;
};

}  // namespace rtype::client
