#include <gtest/gtest.h>
#include "FineTimerResolution.hpp"

/**
 * Given no request for a fine timer resolution
 * When the server requests one
 * Then the system grants it: on Windows the 1 ms resolution, elsewhere nothing
 * needs requesting
 */
TEST(FineTimerResolution, IsGrantedOnTheSystemsTheServerRunsOn) {
  const rtype::server::FineTimerResolution resolution;

  EXPECT_TRUE(resolution.isGranted());
}
