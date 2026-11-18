#pragma once

#include <array>
#include <cstdint>
#include "CasinoController.hpp"
#include "ControllerRoute.hpp"
#include "MasterRouteConstants.hpp"

namespace rtype::master {

/** @brief The routes of the casino and what answers each. */
constexpr std::array CASINO_ROUTES{
    ControllerRoute<CasinoController>{.route = network::LIST_CASINO_ROOMS_ROUTE,
                                      .answer = &CasinoController::listRooms},
    ControllerRoute<CasinoController>{.route = network::READ_CASINO_ROOM_ROUTE,
                                      .answer = &CasinoController::readRoom},
    ControllerRoute<CasinoController>{.route = network::TAKE_OWN_SEAT_ROUTE,
                                      .answer = &CasinoController::takeOwnSeat},
    ControllerRoute<CasinoController>{
        .route = network::LEAVE_OWN_SEAT_ROUTE,
        .answer = &CasinoController::leaveOwnSeat},
    ControllerRoute<CasinoController>{
        .route = network::PLACE_OWN_BETS_ROUTE,
        .answer = &CasinoController::placeOwnBets},
    ControllerRoute<CasinoController>{
        .route = network::READ_OWN_BETS_ROUTE,
        .answer = &CasinoController::readOwnBets}};

/** @brief The settings of the casino while `master.cfg` leaves them out, and
 * their bounds: 3 rooms of 8 seats, rounds of 20 s of bets, 6 s of spin and
 * 5 s of result. */
constexpr std::int64_t DEFAULT_CASINO_ROOMS = 3;
constexpr std::int64_t DEFAULT_CASINO_SEATS = 8;
constexpr std::int64_t DEFAULT_BETTING_SECONDS = 20;
constexpr std::int64_t DEFAULT_SPINNING_SECONDS = 6;
constexpr std::int64_t DEFAULT_RESULT_SECONDS = 5;
constexpr std::int64_t FEWEST_CASINO_ROOMS = 1;
constexpr std::int64_t FEWEST_CASINO_SEATS = 1;
constexpr std::int64_t SHORTEST_BETTING_SECONDS = 5;
constexpr std::int64_t LONGEST_BETTING_SECONDS = 600;
constexpr std::int64_t SHORTEST_SHOW_SECONDS = 1;
constexpr std::int64_t LONGEST_SHOW_SECONDS = 60;

}  // namespace rtype::master
