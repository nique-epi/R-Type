#include "ServerModuleFixture.hpp"
#include <cstdint>
#include <string_view>
#include <vector>
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "UdpPortProbe.hpp"

ServerModuleFixture::ServerModuleFixture()
    : journalPath_(journal_.path().string()),
      arguments_{rtype::logging::FILE_OPTION, journalPath_,
                 rtype::logging::NO_STDERR_OPTION} {}

rtype::logging::LogLaunchOptions ServerModuleFixture::journalOptions() const {
  return rtype::logging::LogLaunchOptions::read(
      arguments_, rtype::logging::SERVER_LOG_FILE_NAME);
}

std::uint16_t ServerModuleFixture::freeUdpPort() { return ::freeUdpPort(); }

bool ServerModuleFixture::portIsFree(std::uint16_t port) {
  return udpPortIsFree(port);
}

const JournalFile& ServerModuleFixture::journal() const { return journal_; }
