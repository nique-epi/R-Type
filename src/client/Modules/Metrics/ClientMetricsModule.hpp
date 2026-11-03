#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "IClock.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "TrafficCounters.hpp"
#include "TrafficRecorder.hpp"

namespace rtype::client {

/**
 * @brief The module that writes the traffic of the client as time goes by: a
 * line in the journal, and rows in a file when it was given one (see
 * network::TrafficRecorder).
 *
 * Its work runs each frame in NETWORK_STAGE, on the thread of the frame loop.
 * No module requires it, so the client runs the same without it.
 */
class ClientMetricsModule final : public engine::IModule {
 public:
  /** @param clock Read once per frame.
   * @param trafficFile Where the rows go; nothing writes no rows. */
  ClientMetricsModule(std::shared_ptr<const engine::IClock> clock,
                      std::optional<std::filesystem::path> trafficFile);

  /** @returns METRICS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns CONNECTION_MODULE_NAME, which provides the traffic. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException when no module provided the traffic. */
  void install(engine::InstallContext& context) override;
  /** @throws network::TrafficFileOpenException when the file cannot be opened.
   */
  void start() override;
  /** @brief Writes the last rows and closes the file. */
  void stop() override;

 private:
  std::shared_ptr<const engine::IClock> clock_;
  std::optional<std::filesystem::path> trafficFile_;
  const network::TrafficCounters* traffic_{nullptr};
  std::unique_ptr<network::TrafficRecorder> recorder_;
};

}  // namespace rtype::client
