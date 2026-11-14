#pragma once

#include "ServerMapScreenFixture.hpp"

namespace rtype::client::testing {

/** @brief The server map over its catalogs, its canvas at the largest text
 * size. */
class LargestServerMapFixture : public ServerMapScreenFixture {
 protected:
  LargestServerMapFixture();
};

}  // namespace rtype::client::testing
