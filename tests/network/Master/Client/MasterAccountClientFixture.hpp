#pragma once

#include <gtest/gtest.h>
#include "FakeAccountMaster.hpp"
#include "MasterAccountClient.hpp"
#include "RecordedRequest.hpp"

/** @brief A stand-in for the routes of the accounts of the master, and a
 * client of it. */
class MasterAccountClientFixture : public ::testing::Test {
 protected:
  MasterAccountClientFixture();

  [[nodiscard]] FakeAccountMaster& master();
  [[nodiscard]] rtype::network::MasterAccountClient& client();

  /** @returns The last request the master received; an empty one before the
   * first. */
  [[nodiscard]] RecordedRequest lastRequest() const;

 private:
  FakeAccountMaster master_;
  rtype::network::MasterAccountClient client_;
};
