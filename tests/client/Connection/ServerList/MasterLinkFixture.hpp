#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "CreatedServer.hpp"
#include "MasterLink.hpp"
#include "RunningMaster.hpp"
#include "ServerListReading.hpp"

namespace rtype::client::testing {

/** @brief A running master and a started link to it, on its own thread. */
class MasterLinkFixture : public ::testing::Test {
 protected:
  MasterLinkFixture();
  ~MasterLinkFixture() override;
  MasterLinkFixture(const MasterLinkFixture&) = delete;
  MasterLinkFixture& operator=(const MasterLinkFixture&) = delete;
  MasterLinkFixture(MasterLinkFixture&&) = delete;
  MasterLinkFixture& operator=(MasterLinkFixture&&) = delete;

  /** @returns The server LINKED_SERVER_NAME, created then registered. */
  [[nodiscard]] network::CreatedServer registeredServer() const;

  /** @returns The reading of the ask for servers numbered @p ask, once the
   * link published it within READING_DEADLINE; nothing otherwise. */
  [[nodiscard]] std::optional<ServerListReading> serversSoon(
      std::uint64_t ask) const;

  [[nodiscard]] RunningMaster& master();
  [[nodiscard]] MasterLink& link();

 private:
  RunningMaster master_;
  MasterLink link_;
};

}  // namespace rtype::client::testing
