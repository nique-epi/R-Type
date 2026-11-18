#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "DeviceLinkStatus.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterDeviceLinkFlow.hpp"

namespace rtype::client::testing {

/** @brief A stand-in for the master that opens FLOW_SECRET as a link, and a
 * started flow to it, on its own thread. */
class MasterDeviceLinkFlowFixture : public ::testing::Test {
 protected:
  MasterDeviceLinkFlowFixture();

  /** @brief Stops the flow before the master it reads goes away. */
  void TearDown() override;

  [[nodiscard]] FakeAccountMaster& master();
  [[nodiscard]] MasterDeviceLinkFlow& flow();

  /** @brief Makes the fake master show its link as @p status. */
  void showLinkAs(network::DeviceLinkStatus status);

  /** @returns The reading of the ask @p ask once it reached @p phase within
   * FLOW_READING_DEADLINE; nothing otherwise. */
  [[nodiscard]] std::optional<DeviceLinkReading> readingSoon(
      std::uint64_t ask, DeviceLinkPhase phase) const;

  /** @returns Whether the fake master received a request to forget the link
   * of FLOW_SECRET within FLOW_READING_DEADLINE. */
  [[nodiscard]] bool forgottenSoon() const;

 private:
  FakeAccountMaster master_;
  MasterDeviceLinkFlow flow_;
};

}  // namespace rtype::client::testing
