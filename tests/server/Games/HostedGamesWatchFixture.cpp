#include "HostedGamesWatchFixture.hpp"
#include <cstdint>
#include <string>
#include "Games.hpp"
#include "HostedGamesWatch.hpp"
#include "HostedGamesWatchTestConstants.hpp"

namespace rtype::server::testing {

HostedGamesWatchFixture::HostedGamesWatchFixture()
    : watch_(latest_, std::string{WATCHED_DIFFICULTY}) {
  watch_.watch(events_);
}

HostedGamesWatch& HostedGamesWatchFixture::watch() { return watch_; }

const Games& HostedGamesWatchFixture::games() const { return games_; }

std::uint64_t HostedGamesWatchFixture::listsPublished() const {
  return latest_.revision();
}

}  // namespace rtype::server::testing
