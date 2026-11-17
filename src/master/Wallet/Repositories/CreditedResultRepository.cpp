#include "CreditedResultRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <string>
#include "EpochSeconds.hpp"
#include "GameCredit.hpp"
#include "WalletRepositoryConstants.hpp"

namespace rtype::master {

CreditedResultRepository::CreditedResultRepository(SQLite::Database& connection)
    : connection_(connection) {}

bool CreditedResultRepository::insert(const GameCredit& credit,
                                      std::int64_t userId) {
  SQLite::Statement insert{connection_,
                           std::string{INSERT_CREDITED_RESULT_STATEMENT}};
  insert.bind(std::string{REFERENCE_PARAMETER}, credit.reference);
  insert.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  insert.bind(std::string{CREDITED_AT_PARAMETER},
              epochSecondsOf(credit.creditedAt));
  return insert.exec() > 0;
}

}  // namespace rtype::master
