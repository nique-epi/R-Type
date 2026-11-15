#include "ServerModuleFixture.hpp"
#include <cstdint>
#include <vector>
#include "LogLaunchOptions.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "UdpPortProbe.hpp"

ServerModuleFixture::ServerModuleFixture()
    : journalPath_(journal_.path().string()),
      arguments_{rtype::logging::FILE_OPTION, journalPath_,
                 rtype::logging::NO_STDERR_OPTION} {}

rtype::logging::LogLaunchOptions ServerModuleFixture::journalOptions() const {
  return logOptionsOf(arguments_, rtype::logging::SERVER_LOG_FILE_NAME);
}

void ServerModuleFixture::writeTheJournalAsJson() {
  arguments_.push_back(rtype::logging::FORMAT_OPTION);
  arguments_.push_back(JSON_JOURNAL_FORMAT);
}

std::uint16_t ServerModuleFixture::freeUdpPort() { return ::freeUdpPort(); }

bool ServerModuleFixture::portIsFree(std::uint16_t port) {
  return udpPortIsFree(port);
}

const JournalFile& ServerModuleFixture::journal() const { return journal_; }
