#include "ServerKeysFixture.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include <string_view>
#include "ServerKeys.hpp"
#include "ServerKeysTestConstants.hpp"

rtype::master::ServerKeys& ServerKeysFixture::keys() { return keys_; }

void ServerKeysFixture::restart() { databaseFile_.restart(); }

bool ServerKeysFixture::databaseFilesHold(std::string_view text) const {
  const std::filesystem::path file = databaseFile_.file();
  for (const std::filesystem::path& part :
       {file, std::filesystem::path{file.string() +
                                    std::string{KEYS_DATABASE_LOG_SUFFIX}}}) {
    std::ifstream stream{part, std::ios::binary};
    const std::string bytes{std::istreambuf_iterator<char>{stream},
                            std::istreambuf_iterator<char>{}};
    if (!std::ranges::search(bytes, text).empty()) {
      return true;
    }
  }
  return false;
}
