#include "ServerModuleFixture.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "LogLaunchOptions.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingLine.hpp"
#include "UdpPortProbe.hpp"

ServerModuleFixture::ServerModuleFixture()
    : journalPath_(journal_.path().string()),
      arguments_{rtype::logging::FILE_OPTION, journalPath_,
                 rtype::logging::NO_STDERR_OPTION} {}

rtype::logging::LogLaunchOptions ServerModuleFixture::journalOptions() const {
  return logOptionsOf(arguments_, rtype::logging::SERVER_LOG_FILE_NAME);
}

std::vector<std::string> ServerModuleFixture::onAnyPort(
    std::vector<std::string> lines) {
  lines.push_back(
      rtype::engine::settingLine(rtype::server::PORT_SETTING_NAME,
                                 std::to_string(rtype::network::ANY_PORT)));
  return lines;
}

bool ServerModuleFixture::portIsFree(std::uint16_t port) {
  return udpPortIsFree(port);
}

const JournalFile& ServerModuleFixture::journal() const { return journal_; }
