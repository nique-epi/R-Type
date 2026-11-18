#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include "DeviceLinkAsks.hpp"
#include "DeviceLinkFlowState.hpp"
#include "DeviceLinkReading.hpp"
#include "IDeviceLinkFlow.hpp"

namespace rtype::client {

/**
 * @brief The sign-in through the web site on a thread of its own:
 * network::MasterDeviceLinkClient blocks for as long as HTTP takes, and the
 * frame loop must never wait on it.
 *
 * The thread sleeps until a newer ask, or until the link it holds is due to
 * be read again: at the interval the master gave, longer after a 429, and
 * from FIRST_UNREACHABLE_WAIT up to LONGEST_UNREACHABLE_WAIT while no master
 * answers. It serves a cancel, then a sign-out, then a new link, and
 * publishes where the link stands. Once approved, it trades the link for the
 * token of a session at once. The secret of the link never leaves the thread.
 *
 * Building it opens nothing; start() builds the client of the master, which
 * checks its url, and starts the thread, which owns it; stop() ends the
 * thread, a call in progress ended within the timeouts of the client.
 */
class MasterDeviceLinkFlow final : public IDeviceLinkFlow {
 public:
  MasterDeviceLinkFlow() = default;
  ~MasterDeviceLinkFlow() override = default;
  MasterDeviceLinkFlow(const MasterDeviceLinkFlow&) = delete;
  MasterDeviceLinkFlow& operator=(const MasterDeviceLinkFlow&) = delete;
  MasterDeviceLinkFlow(MasterDeviceLinkFlow&&) = delete;
  MasterDeviceLinkFlow& operator=(MasterDeviceLinkFlow&&) = delete;

  /** @brief Starts the thread on the master at @p url.
   * @throws network::InvalidMasterUrlException when the url is not
   * `http(s)://<host>[:<port>]`; the thread does not start then. */
  void start(const std::string& url);

  /** @brief Ends the thread, and with it its client. */
  void stop();

  std::uint64_t askLink() override;
  void askCancel() override;
  void askSignOut(const std::string& token) override;
  [[nodiscard]] std::optional<DeviceLinkReading> reading() const override;

 private:
  DeviceLinkAsks asked_;
  DeviceLinkFlowState state_;
  std::jthread thread_;
};

}  // namespace rtype::client
