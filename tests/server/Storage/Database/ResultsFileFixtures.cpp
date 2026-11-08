#include "ResultsFileFixtures.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include "GameResult.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsFileTestConstants.hpp"
#include "StorageConstants.hpp"

namespace rtype::server::testing {

GameResult vanillaGameOf(std::string_view pilot,
                         std::chrono::sys_seconds endedAt) {
  return GameResult{.difficulty = std::string{SAVED_GAME_DIFFICULTY},
                    .playerCount = ONE_PILOT,
                    .endedAt = endedAt,
                    .players = {PlayerResult{.nickname = std::string{pilot},
                                             .score = SAVED_PILOT_SCORE,
                                             .deaths = 0}},
                    .vanilla = true};
}

std::string saveResultIn(const std::filesystem::path& file,
                         const GameResult& result) {
  ResultsDatabase database{file};
  return database.save(result);
}

MasterAnswers answersIn(const std::filesystem::path& file,
                        std::string_view gameId) {
  const SQLite::Database reader{file.string(), SQLite::OPEN_READONLY,
                                storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  SQLite::Statement query{reader, std::string{ANSWERS_OF_A_GAME_QUERY}};
  query.bind(std::string{SAVED_GAME_ID_PARAMETER}, std::string{gameId});
  query.executeStep();
  const int rows = query.getColumn(ROWS_COLUMN).getInt();
  return MasterAnswers{
      .sent = rows > 0 && query.getColumn(SENT_ROWS_COLUMN).getInt() == rows,
      .refused =
          rows > 0 && query.getColumn(REFUSED_ROWS_COLUMN).getInt() == rows};
}

}  // namespace rtype::server::testing
