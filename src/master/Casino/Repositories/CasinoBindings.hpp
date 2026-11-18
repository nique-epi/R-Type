#pragma once

#include <SQLiteCpp/Statement.h>
#include "RoomRound.hpp"
#include "SeatKey.hpp"

namespace rtype::master {

/** @brief Binds the room and the round of @p place to :room and :round of
 * @p statement. */
void bindPlace(SQLite::Statement& statement, network::RoomRound place);

/** @brief Binds the room and the account of @p seat to :room and :user_id of
 * @p statement. */
void bindSeat(SQLite::Statement& statement, SeatKey seat);

}  // namespace rtype::master
