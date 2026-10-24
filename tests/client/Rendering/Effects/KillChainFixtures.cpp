#include "KillChainFixtures.hpp"
#include <vector>
#include "KillChain.hpp"
#include "TimeConstants.hpp"

using rtype::client::KillChain;
using rtype::engine::Duration;

std::vector<bool> bigCatchesOf(int count, Duration gap) {
  KillChain chain;
  std::vector<bool> bigCatches;
  for (int kill = 0; kill < count; ++kill) {
    bigCatches.push_back(chain.noteKill());
    chain.advance(gap);
  }
  return bigCatches;
}
