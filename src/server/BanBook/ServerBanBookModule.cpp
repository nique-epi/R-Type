#include "ServerBanBookModule.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include "BanBook.hpp"
#include "BanBookConstants.hpp"
#include "CurrentSecond.hpp"
#include "IResultsFile.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"

namespace rtype::server {

ServerBanBookModule::ServerBanBookModule()
    : logger_(std::string{BANS_LOGGER_NAME}) {}

std::string ServerBanBookModule::name() const {
  return std::string{BANS_MODULE_NAME};
}

std::vector<std::string> ServerBanBookModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME},
          std::string{SUPERVISED_GAMES_MODULE_NAME}};
}

void ServerBanBookModule::install(engine::InstallContext& context) {
  file_ = &context.require<IResultsFile>();
  context.provide<BanBook>(book_);
}

void ServerBanBookModule::start() {
  const std::filesystem::path file = file_->resultsFile();
  book_.open(file, currentSecond());
  logger_.info(book_.list().bans().size(), " bans in force, kept in ",
               logging::genericText(file));
}

void ServerBanBookModule::stop() { book_.close(); }

}  // namespace rtype::server
