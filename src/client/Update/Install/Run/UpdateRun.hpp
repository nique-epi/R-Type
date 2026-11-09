#pragma once

#include <cstddef>
#include <filesystem>
#include <mutex>
#include <optional>
#include <span>
#include <thread>
#include <vector>
#include "ContentPackDownload.hpp"
#include "IUpdateRun.hpp"
#include "LaunchCommand.hpp"
#include "Logger.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client {

/**
 * @brief An update: building it starts the download of the package on the
 * thread of a ContentPackDownload; advance() then installs it on a thread of
 * its own, once its SHA-256 is the one of the signed manifest, and writes the
 * notes of the release for the next start; restart() starts the new version.
 * Destroying it stops the download and waits for an install in progress.
 */
class UpdateRun final : public IUpdateRun {
 public:
  explicit UpdateRun(UpdateRunParts parts);

  [[nodiscard]] UpdateProgress progress() const override;
  void advance() override;
  [[nodiscard]] bool restart() override;

 private:
  void install(const std::vector<std::byte>& bytes);
  /** @returns The command that starts the new version, once installed. */
  [[nodiscard]] std::optional<LaunchCommand> installedCommand(
      std::span<const std::byte> bytes);
  [[nodiscard]] std::optional<std::filesystem::path> writePackage(
      std::span<const std::byte> bytes) const;
  void end(UpdatePhase phase);

  UpdateRunParts parts_;
  logging::Logger logger_;
  ContentPackDownload download_;
  mutable std::mutex mutex_;
  UpdatePhase phase_{UpdatePhase::Downloading};
  std::optional<LaunchCommand> command_;
  std::jthread installer_;
};

}  // namespace rtype::client
