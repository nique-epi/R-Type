#include "AccountRow.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Statement.h>
#include "Account.hpp"
#include "AccountRole.hpp"
#include "AccountsRepositoryConstants.hpp"
#include "MasterMessageText.hpp"

namespace rtype::master {

network::Account accountIn(const SQLite::Statement& query) {
  return network::Account{
      .id = query.getColumn(ROW_ACCOUNT_ID_COLUMN).getInt64(),
      .nickname = query.getColumn(ROW_NICKNAME_COLUMN).getString(),
      .role =
          network::accountRoleOf(query.getColumn(ROW_ROLE_COLUMN).getString())
              .value_or(network::AccountRole::Player)};
}

}  // namespace rtype::master
