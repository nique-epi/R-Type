#include "CasinoBindings.hpp"
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <string>
#include "CasinoRepositoryConstants.hpp"
#include "RoomRound.hpp"
#include "SeatKey.hpp"

namespace rtype::master {

void bindPlace(SQLite::Statement& statement, network::RoomRound place) {
  statement.bind(std::string{ROOM_PARAMETER}, int{place.room});
  statement.bind(std::string{ROUND_PARAMETER},
                 static_cast<std::int64_t>(place.round));
}

void bindSeat(SQLite::Statement& statement, SeatKey seat) {
  statement.bind(std::string{ROOM_PARAMETER}, int{seat.room});
  statement.bind(std::string{CASINO_USER_ID_PARAMETER}, seat.userId);
}

}  // namespace rtype::master
