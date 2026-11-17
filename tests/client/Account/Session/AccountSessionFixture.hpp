#pragma once

#include <gtest/gtest.h>
#include "AccountSession.hpp"
#include "RecordingAccountLink.hpp"

namespace rtype::client::testing {

/** @brief An account session over a link that answers what the test sets. */
class AccountSessionFixture : public ::testing::Test {
 protected:
  AccountSessionFixture();

  [[nodiscard]] RecordingAccountLink& link();
  [[nodiscard]] AccountSession& session();

  /** @brief Signs in with ACE_TOKEN, and lets the link name aceAccount(). */
  void signInAsAce();

 private:
  RecordingAccountLink link_;
  AccountSession session_;
};

}  // namespace rtype::client::testing
