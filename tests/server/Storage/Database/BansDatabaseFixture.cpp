#include "BansDatabaseFixture.hpp"
#include <filesystem>
#include "BansDatabaseTestConstants.hpp"

namespace rtype::server::testing {

std::filesystem::path BansDatabaseFixture::file() const {
  return folder_.path() / BANS_FILE_NAME;
}

}  // namespace rtype::server::testing
