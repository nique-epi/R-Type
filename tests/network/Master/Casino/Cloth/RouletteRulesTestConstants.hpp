#pragma once

#include <array>
#include <cstdint>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteRulesFixtures.hpp"

namespace rtype::network::testing {

/** @brief The chips of every bet of the tables below. */
constexpr std::uint32_t TEN_CHIPS = 10;

/**
 * @brief For each kind of bet, a number it covers and one it does not, with
 * what ten chips return: 36 for a straight, 18 for a split, 12 for a street,
 * 9 for a corner, 6 for a six line, 3 for a dozen or a column, 2 for an even
 * chance, the stake included. The zero loses every bet but a straight on it.
 */
constexpr std::array<PayoutCase, 39> PAYOUT_CASES{{
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Straight,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 17,
               .payout = 360},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Straight,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 18,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Straight,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 360},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Straight,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 1,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Split,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 18,
               .payout = 180},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Split,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 17,
               .payout = 180},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Split,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 20,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::SplitAcross,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 20,
               .payout = 180},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::SplitAcross,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 18,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Street,
                                  .firstNumber = 16,
                                  .amount = 10},
               .drawn = 18,
               .payout = 120},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Street,
                                  .firstNumber = 16,
                                  .amount = 10},
               .drawn = 19,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Corner,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 21,
               .payout = 90},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Corner,
                                  .firstNumber = 17,
                                  .amount = 10},
               .drawn = 19,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::SixLine,
                                  .firstNumber = 31,
                                  .amount = 10},
               .drawn = 36,
               .payout = 60},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::SixLine,
                                  .firstNumber = 31,
                                  .amount = 10},
               .drawn = 30,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Dozen,
                                  .firstNumber = 13,
                                  .amount = 10},
               .drawn = 24,
               .payout = 30},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Dozen,
                                  .firstNumber = 13,
                                  .amount = 10},
               .drawn = 25,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Dozen,
                                  .firstNumber = 13,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Column,
                                  .firstNumber = 2,
                                  .amount = 10},
               .drawn = 35,
               .payout = 30},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Column,
                                  .firstNumber = 2,
                                  .amount = 10},
               .drawn = 36,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Column,
                                  .firstNumber = 2,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Red,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 36,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Red,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 35,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Red,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Black,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 35,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Black,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 36,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Black,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Even,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 36,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Even,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 35,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Even,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Odd,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 1,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Odd,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 2,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Odd,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Low,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 18,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Low,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 19,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::Low,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::High,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 19,
               .payout = 20},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::High,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 18,
               .payout = 0},
    PayoutCase{.bet = RouletteBet{.kind = RouletteBetKind::High,
                                  .firstNumber = 0,
                                  .amount = 10},
               .drawn = 0,
               .payout = 0},
}};

/** @brief Bets that leave the cloth, cross the end of a street, touch the
 * zero, or name a number where the kind takes none. */
constexpr std::array<RouletteBet, 19> REFUSED_BETS{{
    RouletteBet{
        .kind = RouletteBetKind::Straight, .firstNumber = 37, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Split, .firstNumber = 18, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Split, .firstNumber = 36, .amount = 10},
    RouletteBet{.kind = RouletteBetKind::Split, .firstNumber = 0, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SplitAcross, .firstNumber = 34, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SplitAcross, .firstNumber = 0, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Street, .firstNumber = 17, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Street, .firstNumber = 37, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Corner, .firstNumber = 18, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Corner, .firstNumber = 33, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Corner, .firstNumber = 0, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SixLine, .firstNumber = 34, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SixLine, .firstNumber = 32, .amount = 10},
    RouletteBet{.kind = RouletteBetKind::Dozen, .firstNumber = 2, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Dozen, .firstNumber = 37, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Column, .firstNumber = 4, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Column, .firstNumber = 0, .amount = 10},
    RouletteBet{.kind = RouletteBetKind::Red, .firstNumber = 5, .amount = 10},
    RouletteBet{.kind = RouletteBetKind::High, .firstNumber = 19, .amount = 10},
}};

/** @brief The last bet of each kind that still lies on the cloth. */
constexpr std::array<RouletteBet, 9> PLACED_BETS{{
    RouletteBet{
        .kind = RouletteBetKind::Straight, .firstNumber = 36, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Split, .firstNumber = 35, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SplitAcross, .firstNumber = 33, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Street, .firstNumber = 34, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Corner, .firstNumber = 32, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::SixLine, .firstNumber = 31, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Dozen, .firstNumber = 25, .amount = 10},
    RouletteBet{
        .kind = RouletteBetKind::Column, .firstNumber = 3, .amount = 10},
    RouletteBet{.kind = RouletteBetKind::Odd, .firstNumber = 0, .amount = 10},
}};

/** @brief A number past the wheel. */
constexpr std::uint8_t NUMBER_PAST_THE_WHEEL = 37;

}  // namespace rtype::network::testing
