#include "MasterDeviceLinkFlow.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkAsks.hpp"
#include "DeviceLinkFlowConstants.hpp"
#include "DeviceLinkFlowState.hpp"
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "HttpStatusConstants.hpp"
#include "MasterCallsConstants.hpp"
#include "MasterDeviceLinkClient.hpp"
#include "NetworkException.hpp"

namespace rtype::client {

namespace {

using SteadyClock = std::chrono::steady_clock;

/** @brief What the thread knows of the link it holds, which only it reads
 * and writes: the secret, where the link stands, when it reads it next, and
 * the asks it served; and where it publishes. */
struct LinkInProgress {
  /** @brief What the thread shares with the frame loop, where it publishes. */
  std::reference_wrapper<DeviceLinkFlowState> state;
  std::optional<std::string> secret;
  DeviceLinkReading reading;
  SteadyClock::time_point nextRead;
  std::chrono::seconds interval{};
  std::chrono::seconds unreachableWait{FIRST_UNREACHABLE_WAIT};
  DeviceLinkAsks served;
};

/** @returns How long the thread may sleep before it must read its link: an
 * ask wakes it sooner. */
SteadyClock::duration waitOf(const LinkInProgress& work) {
  if (!work.secret.has_value()) {
    return MASTER_ASK_WAIT;
  }
  return std::max(work.nextRead - SteadyClock::now(),
                  SteadyClock::duration::zero());
}

/** @brief Ends the link of @p work at @p phase: it is read no more. */
void endLink(LinkInProgress& work, DeviceLinkPhase phase) {
  work.secret.reset();
  work.reading.phase = phase;
  work.state.get().reading.publish(work.reading);
}

/** @brief Forgets the link @p work holds, on the master too; a master that
 * does not answer forgets it when it expires. */
void forgetLink(network::MasterDeviceLinkClient& client, LinkInProgress& work) {
  if (!work.secret.has_value()) {
    return;
  }
  try {
    client.deleteOwnDeviceLink(*work.secret);
  } catch (const network::NetworkException&) {
    work.secret.reset();
    return;
  }
  work.secret.reset();
}

/** @returns The reading of the link @p created, numbered @p ask. */
DeviceLinkReading readingOf(const network::CreatedDeviceLink& created,
                            std::uint64_t ask) {
  return DeviceLinkReading{
      .ask = ask,
      .phase = DeviceLinkPhase::Waiting,
      .userCode = created.userCode,
      .verificationUri = created.verificationUri,
      .verificationUriComplete = created.verificationUriComplete,
      .expiresAt = created.expiresAt,
      .token = {}};
}

/** @returns The reading of the ask @p ask before the master answered it. */
DeviceLinkReading requestedReading(std::uint64_t ask) {
  return DeviceLinkReading{.ask = ask,
                           .phase = DeviceLinkPhase::Requesting,
                           .userCode = {},
                           .verificationUri = {},
                           .verificationUriComplete = {},
                           .expiresAt = {},
                           .token = {}};
}

/** @brief Asks the master for a new link, for the ask of the reading of
 * @p work, in place of the link it holds, and publishes where it stands. */
void openLink(network::MasterDeviceLinkClient& client, LinkInProgress& work) {
  const std::uint64_t ask = work.reading.ask;
  forgetLink(client, work);
  work.reading = requestedReading(ask);
  try {
    const network::CreatedDeviceLink created = client.createDeviceLink();
    work.secret = created.secret;
    work.interval = created.interval;
    work.unreachableWait = FIRST_UNREACHABLE_WAIT;
    work.nextRead = SteadyClock::now() + created.interval;
    work.reading = readingOf(created, ask);
    work.state.get().reading.publish(work.reading);
  } catch (const network::MasterRefusedException&) {
    endLink(work, DeviceLinkPhase::Refused);
  } catch (const network::NetworkException&) {
    endLink(work, DeviceLinkPhase::Unreachable);
  }
}

/** @brief Closes the session of the token of @p asks; a master that does not
 * answer lets it expire. */
void closeSession(network::MasterDeviceLinkClient& client,
                  const DeviceLinkAsks& asks) {
  try {
    client.deleteOwnSession(asks.signOutToken);
  } catch (const network::NetworkException&) {
    return;
  }
}

/**
 * @brief Serves each ask of @p asks newer than the one @p work served: a
 * sign-out, then a new link unless it was cancelled already, then the cancel
 * of the link it holds. A cancel names the newest link asked when it was
 * made, so a link asked after it is opened, and one asked before it is not,
 * whatever order the two asks reach the thread in.
 */
void serveAsks(network::MasterDeviceLinkClient& client,
               const DeviceLinkAsks& asks, LinkInProgress& work) {
  if (asks.signOutAsk > work.served.signOutAsk) {
    closeSession(client, asks);
    work.served.signOutAsk = asks.signOutAsk;
  }
  if (asks.linkAsk > work.served.linkAsk &&
      asks.cancelledLinkAsk < asks.linkAsk) {
    work.reading = requestedReading(asks.linkAsk);
    openLink(client, work);
  }
  work.served.linkAsk = asks.linkAsk;
  if (asks.cancelledLinkAsk >= work.reading.ask) {
    forgetLink(client, work);
  }
}

/** @returns Where a link stands once the master refused to trade it with
 * @p status: refused, expired, or still to read when the answer says
 * nothing final. */
std::optional<DeviceLinkPhase> phaseOfRefusedTrade(int status) {
  switch (status) {
    case network::FORBIDDEN_STATUS:
      return DeviceLinkPhase::Denied;
    case network::GONE_STATUS:
    case network::UNAUTHORIZED_STATUS:
      return DeviceLinkPhase::Expired;
    default:
      return std::nullopt;
  }
}

/** @brief Trades the approved link of @p work for the token of a session,
 * and publishes it; a link the master no longer trades ends as it says. */
void tradeLink(network::MasterDeviceLinkClient& client, LinkInProgress& work) {
  if (!work.secret.has_value()) {
    return;
  }
  try {
    work.reading.token = client.createSessionOfDeviceLink(*work.secret).token;
    endLink(work, DeviceLinkPhase::SignedIn);
  } catch (const network::MasterRefusedException& refused) {
    const std::optional<DeviceLinkPhase> phase =
        phaseOfRefusedTrade(refused.status());
    if (phase.has_value()) {
      endLink(work, *phase);
    }
  } catch (const network::NetworkException&) {
    work.nextRead = SteadyClock::now() + work.unreachableWait;
  }
}

/** @brief Acts on @p status, the state the master gave of the link of
 * @p work: waits, trades it, or ends it. */
void followStatus(network::MasterDeviceLinkClient& client,
                  network::DeviceLinkStatus status, LinkInProgress& work) {
  switch (status) {
    case network::DeviceLinkStatus::Pending:
      return;
    case network::DeviceLinkStatus::Approved:
      tradeLink(client, work);
      return;
    case network::DeviceLinkStatus::Denied:
      endLink(work, DeviceLinkPhase::Denied);
      return;
    case network::DeviceLinkStatus::Expired:
      endLink(work, DeviceLinkPhase::Expired);
      return;
  }
}

/** @brief Reads the link of @p work when it is due, and follows what the
 * master says of it; waits longer after each failure to reach it. */
void readDueLink(network::MasterDeviceLinkClient& client,
                 LinkInProgress& work) {
  if (!work.secret.has_value() || SteadyClock::now() < work.nextRead) {
    return;
  }
  work.nextRead = SteadyClock::now() + work.interval;
  try {
    const network::DeviceLinkState read =
        client.readOwnDeviceLink(*work.secret);
    work.unreachableWait = FIRST_UNREACHABLE_WAIT;
    followStatus(client, read.status, work);
  } catch (const network::MasterRefusedException& refused) {
    if (refused.status() == network::UNAUTHORIZED_STATUS) {
      endLink(work, DeviceLinkPhase::Expired);
    }
  } catch (const network::NetworkException&) {
    work.nextRead = SteadyClock::now() + work.unreachableWait;
    work.unreachableWait =
        std::min(work.unreachableWait * UNREACHABLE_WAIT_GROWTH,
                 LONGEST_UNREACHABLE_WAIT);
  }
}

/** @brief The loop of the thread: serves the asks, then reads the link when
 * it is due, until @p stop is requested. */
void runFlow(const std::stop_token& stop,
             network::MasterDeviceLinkClient& client,
             DeviceLinkFlowState& state) {
  LinkInProgress work{.state = state,
                      .secret = {},
                      .reading = {},
                      .nextRead = {},
                      .interval = {},
                      .unreachableWait = FIRST_UNREACHABLE_WAIT,
                      .served = {}};
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    seen = state.asks.waitForNewer(seen, waitOf(work), stop);
    if (const std::optional<DeviceLinkAsks> asks = state.asks.value()) {
      serveAsks(client, *asks, work);
    }
    readDueLink(client, work);
  }
}

}  // namespace

void MasterDeviceLinkFlow::start(const std::string& url) {
  auto client = std::make_unique<network::MasterDeviceLinkClient>(url);
  thread_ = std::jthread(
      [this, owned = std::move(client)](const std::stop_token& stop) {
        runFlow(stop, *owned, state_);
      });
}

void MasterDeviceLinkFlow::stop() {
  thread_.request_stop();
  if (thread_.joinable()) {
    thread_.join();
  }
}

std::uint64_t MasterDeviceLinkFlow::askLink() {
  ++asked_.linkAsk;
  state_.asks.publish(asked_);
  return asked_.linkAsk;
}

void MasterDeviceLinkFlow::askCancel() {
  asked_.cancelledLinkAsk = asked_.linkAsk;
  state_.asks.publish(asked_);
}

void MasterDeviceLinkFlow::askSignOut(const std::string& token) {
  ++asked_.signOutAsk;
  asked_.signOutToken = token;
  state_.asks.publish(asked_);
}

std::optional<DeviceLinkReading> MasterDeviceLinkFlow::reading() const {
  return state_.reading.value();
}

}  // namespace rtype::client
