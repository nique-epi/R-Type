#include "UserRow.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Statement.h>
#include "AccountsRepositoryConstants.hpp"
#include "MasterMessageText.hpp"
#include "User.hpp"
#include "UserRole.hpp"

namespace rtype::master {

network::User userIn(const SQLite::Statement& query) {
  return network::User{
      .id = query.getColumn(ROW_USER_ID_COLUMN).getInt64(),
      .nickname = query.getColumn(ROW_NICKNAME_COLUMN).getString(),
      .role = network::userRoleOf(query.getColumn(ROW_ROLE_COLUMN).getString())
                  .value_or(network::UserRole::Player)};
}

}  // namespace rtype::master
