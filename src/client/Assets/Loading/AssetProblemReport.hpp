#pragma once

#include <string_view>
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief What went wrong while assets of a library were loaded, gathered so
 * one error names every asset that could not be loaded. */
class AssetProblemReport {
 public:
  /** @param library Names the files of the ids; it must outlive the
   * report. */
  explicit AssetProblemReport(const AssetLibrary& library);

  /** @brief Writes down what went wrong with @p assetId: the id when it breaks
   * the asset id rule, its file when it is missing or unreadable, nothing when
   * @p result is Loaded. */
  void record(std::string_view assetId, AssetLoadResult result);

  /** @throws AssetLoadingException naming every asset written down, when
   * there is one. */
  void throwIfAny() const;

 private:
  const AssetLibrary* library_;
  AssetProblems problems_;
};

}  // namespace rtype::client
