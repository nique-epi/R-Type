#pragma once

#include <Velopack.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include "IReleaseUpdater.hpp"
#include "ReleaseFeedSource.hpp"
#include "ReleaseOffer.hpp"

namespace rtype::client {

/**
 * @brief The updates of the client, as Velopack checks, downloads and applies
 * them, from the feeds a ReleaseFeedSource reads. Every error of Velopack,
 * which throws std::runtime_error, leaves it as an UpdateException. Its calls
 * follow one another, as IReleaseUpdater says: the check publishes what it
 * found under the lock of UpdateCheck before the update starts, so the release
 * newerRelease() keeps needs no lock of its own.
 */
class VelopackUpdater final : public IReleaseUpdater {
 public:
  /** @throws UpdateException when the client was not installed by Velopack:
   * run from a build folder, Velopack finds no package around it. */
  explicit VelopackUpdater(std::unique_ptr<ReleaseFeedSource> source);

  [[nodiscard]] std::string currentVersion() override;
  [[nodiscard]] std::optional<ReleaseOffer> newerRelease() override;
  void download(const std::function<void(int)>& progress) override;
  void applyOnExit() override;
  void stop() override;

 private:
  [[nodiscard]] Velopack::UpdateInfo found() const;

  ReleaseFeedSource* source_;
  std::unique_ptr<Velopack::UpdateManager> manager_;
  std::optional<Velopack::UpdateInfo> found_;
};

}  // namespace rtype::client
