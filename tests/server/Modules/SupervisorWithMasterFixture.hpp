#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "RunningMaster.hpp"
#include "SupervisorModulesFixture.hpp"

/**
 * @brief The supervisor of SupervisorModulesFixture next to a running master
 * it may register with.
 */
class SupervisorWithMasterFixture : public SupervisorModulesFixture {
 protected:
  [[nodiscard]] const RunningMaster& master() const;

  /** @returns The address the master lists for the server @p serverId once it
   * registered, asked every LISTING_WAIT_STEP; nothing when it did not within
   * REGISTRATION_DEADLINE. */
  [[nodiscard]] std::optional<std::string> listedAddressSoon(
      std::int64_t serverId) const;

 private:
  RunningMaster master_;
};
