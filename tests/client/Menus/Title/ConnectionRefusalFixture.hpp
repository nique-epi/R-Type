#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include <tuple>
#include "ConnectionRefused.hpp"
#include "ConnectionScreenFixture.hpp"

/** @brief A connection screen whose attempt the server refused, for one
 * reason, and the line it must show. */
class ConnectionRefusalFixture
    : public ConnectionScreenFixture,
      public ::testing::WithParamInterface<
          std::tuple<rtype::network::RefusalReason, std::string_view>> {};
