#pragma once

#include "ConnectionScreenFixture.hpp"

/** @brief A connection screen asking for a server, its canvas at the largest
 * text size. */
class LargestConnectionFixture : public ConnectionScreenFixture {
 protected:
  LargestConnectionFixture();
};
