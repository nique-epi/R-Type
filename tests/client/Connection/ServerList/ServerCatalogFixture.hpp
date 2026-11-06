#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "Endpoint.hpp"
#include "ListedServer.hpp"
#include "RecordingMasterReads.hpp"
#include "RecordingServerPingProbe.hpp"
#include "ServerCatalog.hpp"
#include "ServerListing.hpp"
#include "ServerStatus.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A server of a list, as the tests describe it. */
struct ListedServerSketch {
  std::int64_t id{};
  std::string_view address;
  network::ServerStatus status{network::ServerStatus::Online};
};

/** @brief A catalog over a master and a probe the tests answer for. */
class ServerCatalogFixture : public ::testing::Test {
 protected:
  /** @returns A listed server of this version, its game port
   * SERVER_UDP_PORT. */
  [[nodiscard]] static network::ListedServer listed(ListedServerSketch sketch);
  [[nodiscard]] static network::Endpoint endpointAt(std::string_view address);

  /** @brief Refreshes, has the master answer @p listing, and runs a frame. */
  void listAndUpdate(const network::ServerListing& listing);
  /** @brief Has @p address answer the round with @p ping. */
  void answerFrom(std::string_view address, std::chrono::microseconds ping);
  void update(engine::Duration elapsed);

  [[nodiscard]] ServerCatalog& catalog();
  [[nodiscard]] RecordingMasterReads& master();
  [[nodiscard]] const RecordingServerPingProbe& pings() const;

 private:
  RecordingMasterReads master_;
  RecordingServerPingProbe pings_;
  ServerCatalog catalog_{master_, pings_};
};

}  // namespace rtype::client::testing
