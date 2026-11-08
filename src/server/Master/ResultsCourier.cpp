#include "ResultsCourier.hpp"
#include <chrono>
#include <exception>
#include <filesystem>
#include <stop_token>
#include <string>
#include <utility>
#include <vector>
#include "LastProblem.hpp"
#include "MasterClient.hpp"
#include "MasterMessages.hpp"
#include "NetworkException.hpp"
#include "PathText.hpp"
#include "ResultDelivery.hpp"
#include "ResultsDatabase.hpp"
#include "ServerConstants.hpp"
#include "ServerMasterConstants.hpp"
#include "UnsentGame.hpp"

namespace rtype::server {

ResultsCourier::ResultsCourier(std::filesystem::path file,
                               network::MasterClient& client,
                               LastProblem& problems)
    : file_(std::move(file)),
      client_(client),
      logger_(std::string{SERVER_LOGGER_NAME}),
      problems_(problems) {}

void ResultsCourier::sendWaitingResults(const std::stop_token& stop) {
  ResultsDatabase* const database = openedDatabase();
  if (database == nullptr) {
    return;
  }
  for (const UnsentGame& game : waitingGames(*database)) {
    if (stop.stop_requested()) {
      return;
    }
    const ResultDelivery delivery = deliver(game);
    if (delivery == ResultDelivery::Later) {
      return;
    }
    keep(*database, game.gameId, delivery);
  }
}

ResultsDatabase* ResultsCourier::openedDatabase() {
  if (!database_.has_value()) {
    try {
      database_.emplace(file_);
    } catch (const std::exception& error) {
      problems_.tell("the results of " + logging::genericText(file_) +
                     " wait: " + error.what());
      return nullptr;
    }
  }
  return &*database_;
}

std::vector<UnsentGame> ResultsCourier::waitingGames(
    const ResultsDatabase& database) {
  try {
    return database.unsentVanillaGames(MAXIMUM_RESULTS_PER_ROUND);
  } catch (const std::exception& error) {
    problems_.tell(std::string{"the waiting results cannot be read: "} +
                   error.what());
    return {};
  }
}

ResultDelivery ResultsCourier::deliver(const UnsentGame& game) {
  try {
    client_.sendResult(game.gameId, finishedGameOf(game.result));
    logger_.info("the master keeps the result of game ", game.gameId);
    problems_.forget();
    return ResultDelivery::Accepted;
  } catch (const network::MasterRefusedException& refusal) {
    if (refusal.status() == FOR_GOOD_REFUSAL_STATUS) {
      logger_.error("the master refuses the result of game ", game.gameId,
                    " for good, it is not sent again: ", refusal.what());
      return ResultDelivery::Refused;
    }
    problems_.tell(refusal.what());
  } catch (const network::MasterUnreachableException& silence) {
    problems_.tell(silence.what());
  }
  return ResultDelivery::Later;
}

void ResultsCourier::keep(ResultsDatabase& database, const std::string& gameId,
                          ResultDelivery delivery) {
  const std::chrono::sys_seconds now = std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
  try {
    if (delivery == ResultDelivery::Accepted) {
      database.markSent(gameId, now);
    } else {
      database.markRefused(gameId, now);
    }
  } catch (const std::exception& error) {
    problems_.tell("the answer of the master on game " + gameId +
                   " is not kept, it is sent again: " + error.what());
  }
}

}  // namespace rtype::server
