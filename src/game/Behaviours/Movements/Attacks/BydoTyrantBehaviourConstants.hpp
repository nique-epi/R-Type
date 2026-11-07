#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ActionSettingConstants.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "PlayfieldConstants.hpp"
#include "TyrantAttack.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The settings of the Bydo Tyrant, in logical units and milliseconds:
 * the column it rests at (`rest_x`) and its rest between two attacks
 * (`rest_ms`); its arrival, which slides it in (`slide_ms`), roars
 * (`roar_ms`) and settles (`settle_ms`, 0 to skip); the signal, strike and
 * recover of each attack, `<attack>_signal_ms`, `<attack>_strike_ms`,
 * `<attack>_recover_ms` (see ActionSettingConstants.hpp); the point its
 * tentacles lash (`melee_x`, `melee_y`, from its center) and how near that
 * point a ship makes them lash (`melee_reach`); how long it holds still as
 * it enrages (`rage_ms`), and the share of a calm signal its enraged
 * variants keep (`rage_signal_percent`).
 */
constexpr std::string_view TYRANT_REST_X_SETTING_NAME = "rest_x";
constexpr std::string_view TYRANT_SLIDE_SETTING_NAME = "slide_ms";
constexpr std::string_view TYRANT_ROAR_SETTING_NAME = "roar_ms";
constexpr std::string_view TYRANT_SETTLE_SETTING_NAME = "settle_ms";
constexpr std::string_view TYRANT_BURST_SIGNAL_SETTING_NAME = "burst_signal_ms";
constexpr std::string_view TYRANT_BURST_STRIKE_SETTING_NAME = "burst_strike_ms";
constexpr std::string_view TYRANT_BURST_RECOVER_SETTING_NAME =
    "burst_recover_ms";
constexpr std::string_view TYRANT_BEAM_SIGNAL_SETTING_NAME = "beam_signal_ms";
constexpr std::string_view TYRANT_BEAM_STRIKE_SETTING_NAME = "beam_strike_ms";
constexpr std::string_view TYRANT_BEAM_RECOVER_SETTING_NAME = "beam_recover_ms";
constexpr std::string_view TYRANT_BROOD_SIGNAL_SETTING_NAME = "brood_signal_ms";
constexpr std::string_view TYRANT_BROOD_STRIKE_SETTING_NAME = "brood_strike_ms";
constexpr std::string_view TYRANT_BROOD_RECOVER_SETTING_NAME =
    "brood_recover_ms";
constexpr std::string_view TYRANT_TENTACLE_SIGNAL_SETTING_NAME =
    "tentacle_signal_ms";
constexpr std::string_view TYRANT_TENTACLE_STRIKE_SETTING_NAME =
    "tentacle_strike_ms";
constexpr std::string_view TYRANT_TENTACLE_RECOVER_SETTING_NAME =
    "tentacle_recover_ms";
constexpr std::string_view TYRANT_MELEE_X_SETTING_NAME = "melee_x";
constexpr std::string_view TYRANT_MELEE_Y_SETTING_NAME = "melee_y";
constexpr std::string_view TYRANT_MELEE_REACH_SETTING_NAME = "melee_reach";
constexpr std::string_view TYRANT_RAGE_SETTING_NAME = "rage_ms";
constexpr std::string_view TYRANT_RAGE_SIGNAL_SETTING_NAME =
    "rage_signal_percent";
constexpr std::string_view TYRANT_SWEEP_SIGNAL_SETTING_NAME = "sweep_signal_ms";
constexpr std::string_view TYRANT_SWEEP_STRIKE_SETTING_NAME = "sweep_strike_ms";
constexpr std::string_view TYRANT_SWEEP_RECOVER_SETTING_NAME =
    "sweep_recover_ms";
constexpr std::string_view TYRANT_SPORES_SIGNAL_SETTING_NAME =
    "spores_signal_ms";
constexpr std::string_view TYRANT_SPORES_STRIKE_SETTING_NAME =
    "spores_strike_ms";
constexpr std::string_view TYRANT_SPORES_RECOVER_SETTING_NAME =
    "spores_recover_ms";
constexpr std::string_view TYRANT_WALL_SIGNAL_SETTING_NAME = "wall_signal_ms";
constexpr std::string_view TYRANT_WALL_STRIKE_SETTING_NAME = "wall_strike_ms";
constexpr std::string_view TYRANT_WALL_RECOVER_SETTING_NAME = "wall_recover_ms";
constexpr std::string_view TYRANT_CHARGE_SIGNAL_SETTING_NAME =
    "charge_signal_ms";
constexpr std::string_view TYRANT_CHARGE_STRIKE_SETTING_NAME =
    "charge_strike_ms";
constexpr std::string_view TYRANT_CHARGE_RECOVER_SETTING_NAME =
    "charge_recover_ms";

/** @brief Where its melee point may be from its center, in logical units:
 * within its 512-unit frame; and the share of a signal, in percent. */
constexpr engine::WholeNumberRange TYRANT_MELEE_OFFSET_RANGE{.lowest = -512,
                                                             .highest = 512};
constexpr engine::WholeNumberRange TYRANT_SIGNAL_PERCENT_RANGE{.lowest = 1,
                                                               .highest = 100};

constexpr std::array BYDO_TYRANT_SETTINGS{
    BehaviourSetting{.name = TYRANT_REST_X_SETTING_NAME,
                     .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = REST_SETTING_NAME, .range = REST_RANGE},
    BehaviourSetting{.name = TYRANT_SLIDE_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_ROAR_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_SETTLE_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_BURST_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BURST_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BURST_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_BEAM_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BEAM_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BEAM_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_BROOD_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BROOD_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_BROOD_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_TENTACLE_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_TENTACLE_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_TENTACLE_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_MELEE_X_SETTING_NAME,
                     .range = TYRANT_MELEE_OFFSET_RANGE},
    BehaviourSetting{.name = TYRANT_MELEE_Y_SETTING_NAME,
                     .range = TYRANT_MELEE_OFFSET_RANGE},
    BehaviourSetting{.name = TYRANT_MELEE_REACH_SETTING_NAME,
                     .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = TYRANT_RAGE_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_RAGE_SIGNAL_SETTING_NAME,
                     .range = TYRANT_SIGNAL_PERCENT_RANGE},
    BehaviourSetting{.name = TYRANT_SWEEP_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_SWEEP_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_SWEEP_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_SPORES_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_SPORES_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_SPORES_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_WALL_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_WALL_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_WALL_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE},
    BehaviourSetting{.name = TYRANT_CHARGE_SIGNAL_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_CHARGE_STRIKE_SETTING_NAME,
                     .range = PHASE_RANGE},
    BehaviourSetting{.name = TYRANT_CHARGE_RECOVER_SETTING_NAME,
                     .range = RECOVER_RANGE}};

/** @brief Where each setting is among the values of BehaviourView::settings.
 * The strike and the recover of an attack follow its signal (see
 * timingsAt()); the roar and the settling of the arrival follow its slide. */
constexpr std::size_t TYRANT_REST_X_SETTING = 0;
constexpr std::size_t TYRANT_REST_SETTING = 1;
constexpr std::size_t TYRANT_SLIDE_SETTING = 2;
constexpr std::size_t TYRANT_BURST_SIGNAL_SETTING = 5;
constexpr std::size_t TYRANT_BEAM_SIGNAL_SETTING = 8;
constexpr std::size_t TYRANT_BROOD_SIGNAL_SETTING = 11;
constexpr std::size_t TYRANT_TENTACLE_SIGNAL_SETTING = 14;
constexpr std::size_t TYRANT_MELEE_X_SETTING = 17;
constexpr std::size_t TYRANT_MELEE_Y_SETTING = 18;
constexpr std::size_t TYRANT_MELEE_REACH_SETTING = 19;
constexpr std::size_t TYRANT_RAGE_SETTING = 20;
constexpr std::size_t TYRANT_RAGE_SIGNAL_SETTING = 21;
constexpr std::size_t TYRANT_SWEEP_SIGNAL_SETTING = 22;
constexpr std::size_t TYRANT_SPORES_SIGNAL_SETTING = 25;
constexpr std::size_t TYRANT_WALL_SIGNAL_SETTING = 28;
constexpr std::size_t TYRANT_CHARGE_SIGNAL_SETTING = 31;

static_assert(BYDO_TYRANT_SETTINGS[TYRANT_REST_X_SETTING].name ==
                  TYRANT_REST_X_SETTING_NAME,
              "TYRANT_REST_X_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_REST_SETTING].name ==
                  REST_SETTING_NAME,
              "TYRANT_REST_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_SLIDE_SETTING].name ==
                  TYRANT_SLIDE_SETTING_NAME,
              "TYRANT_SLIDE_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_BURST_SIGNAL_SETTING].name ==
                  TYRANT_BURST_SIGNAL_SETTING_NAME,
              "TYRANT_BURST_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_BEAM_SIGNAL_SETTING].name ==
                  TYRANT_BEAM_SIGNAL_SETTING_NAME,
              "TYRANT_BEAM_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_BROOD_SIGNAL_SETTING].name ==
                  TYRANT_BROOD_SIGNAL_SETTING_NAME,
              "TYRANT_BROOD_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_TENTACLE_SIGNAL_SETTING].name ==
                  TYRANT_TENTACLE_SIGNAL_SETTING_NAME,
              "TYRANT_TENTACLE_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_MELEE_X_SETTING].name ==
                  TYRANT_MELEE_X_SETTING_NAME,
              "TYRANT_MELEE_X_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_MELEE_Y_SETTING].name ==
                  TYRANT_MELEE_Y_SETTING_NAME,
              "TYRANT_MELEE_Y_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_MELEE_REACH_SETTING].name ==
                  TYRANT_MELEE_REACH_SETTING_NAME,
              "TYRANT_MELEE_REACH_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_RAGE_SETTING].name ==
                  TYRANT_RAGE_SETTING_NAME,
              "TYRANT_RAGE_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_RAGE_SIGNAL_SETTING].name ==
                  TYRANT_RAGE_SIGNAL_SETTING_NAME,
              "TYRANT_RAGE_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_SWEEP_SIGNAL_SETTING].name ==
                  TYRANT_SWEEP_SIGNAL_SETTING_NAME,
              "TYRANT_SWEEP_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_SPORES_SIGNAL_SETTING].name ==
                  TYRANT_SPORES_SIGNAL_SETTING_NAME,
              "TYRANT_SPORES_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_WALL_SIGNAL_SETTING].name ==
                  TYRANT_WALL_SIGNAL_SETTING_NAME,
              "TYRANT_WALL_SIGNAL_SETTING names its setting");
static_assert(BYDO_TYRANT_SETTINGS[TYRANT_CHARGE_SIGNAL_SETTING].name ==
                  TYRANT_CHARGE_SIGNAL_SETTING_NAME,
              "TYRANT_CHARGE_SIGNAL_SETTING names its setting");

/** @brief The attack numbers of the Tyrant, which its sheet animates and
 * times its hurt parts and volleys with: its arrival, its four calm attacks,
 * their enraged variants TYRANT_ENRAGED_SHIFT above them (so the client
 * shows the enraged skin), and its four enraged-only attacks. Its rage is
 * the `rage_attack` of its `[gameplay.boss]` (9), in the phase Enraged,
 * which the behaviour recognises by its phase alone. */
constexpr std::uint8_t TYRANT_ARRIVAL_ATTACK = 0;
constexpr std::uint8_t TYRANT_BURST_ATTACK = 1;
constexpr std::uint8_t TYRANT_BEAM_ATTACK = 2;
constexpr std::uint8_t TYRANT_BROOD_ATTACK = 3;
constexpr std::uint8_t TYRANT_TENTACLE_ATTACK = 4;
constexpr std::uint8_t TYRANT_ENRAGED_SHIFT = 10;
constexpr auto TYRANT_ENRAGED_BURST_ATTACK =
    static_cast<std::uint8_t>(TYRANT_BURST_ATTACK + TYRANT_ENRAGED_SHIFT);
constexpr auto TYRANT_ENRAGED_BEAM_ATTACK =
    static_cast<std::uint8_t>(TYRANT_BEAM_ATTACK + TYRANT_ENRAGED_SHIFT);
constexpr auto TYRANT_ENRAGED_BROOD_ATTACK =
    static_cast<std::uint8_t>(TYRANT_BROOD_ATTACK + TYRANT_ENRAGED_SHIFT);
constexpr auto TYRANT_ENRAGED_TENTACLE_ATTACK =
    static_cast<std::uint8_t>(TYRANT_TENTACLE_ATTACK + TYRANT_ENRAGED_SHIFT);
constexpr std::uint8_t TYRANT_SWEEP_ATTACK = 15;
constexpr std::uint8_t TYRANT_SPORES_ATTACK = 16;
constexpr std::uint8_t TYRANT_WALL_ATTACK = 17;
constexpr std::uint8_t TYRANT_CHARGE_ATTACK = 18;

/** @brief The attacks that lay their bodies out on the playfield from the
 * row it arrived on (its broods, its spores and its wall): it goes back to
 * that row before them. */
constexpr std::array TYRANT_LAID_OUT_ATTACKS{
    TYRANT_BROOD_ATTACK, TYRANT_ENRAGED_BROOD_ATTACK, TYRANT_SPORES_ATTACK,
    TYRANT_WALL_ATTACK};

/** @brief Between two attacks it drifts up or down, never during one (its
 * attacks start from where it stands), at TYRANT_DRIFT_SPEED units per
 * second at most, within TYRANT_DRIFT_SPAN units of the row it arrived on:
 * a 512-unit body keeps the play area on both sides. It heads, in turn, for
 * the row of the nearest ship and for a row drawn from its count of actions
 * (TYRANT_DRIFT_TURNS), so with four players it does not follow one. */
constexpr float TYRANT_DRIFT_SPEED = 240.0F;
constexpr float TYRANT_DRIFT_SPAN = 200.0F;
constexpr std::uint32_t TYRANT_DRIFT_TURNS = 2;
/** @brief A drawn row is a share from 0 to 1 (the high 16 bits of the count
 * times a Knuth multiplier), stretched over both sides of the row it arrived
 * on: TYRANT_DRIFT_SIDES times the share, less one, from -1 to 1. */
constexpr std::uint32_t TYRANT_DRIFT_DRAW_MULTIPLIER = 2654435761U;
constexpr int TYRANT_DRIFT_DRAW_SHIFT = 16;
constexpr float TYRANT_DRIFT_SIDES = 2.0F;

/** @brief Every attack of the Tyrant and where its timings are. */
constexpr std::array TYRANT_ATTACKS{
    TyrantAttack{.attack = TYRANT_ARRIVAL_ATTACK,
                 .signalSetting = TYRANT_SLIDE_SETTING},
    TyrantAttack{.attack = TYRANT_BURST_ATTACK,
                 .signalSetting = TYRANT_BURST_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_BEAM_ATTACK,
                 .signalSetting = TYRANT_BEAM_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_BROOD_ATTACK,
                 .signalSetting = TYRANT_BROOD_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_TENTACLE_ATTACK,
                 .signalSetting = TYRANT_TENTACLE_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_ENRAGED_BURST_ATTACK,
                 .signalSetting = TYRANT_BURST_SIGNAL_SETTING,
                 .hastened = true},
    TyrantAttack{.attack = TYRANT_ENRAGED_BEAM_ATTACK,
                 .signalSetting = TYRANT_BEAM_SIGNAL_SETTING,
                 .hastened = true},
    TyrantAttack{.attack = TYRANT_ENRAGED_BROOD_ATTACK,
                 .signalSetting = TYRANT_BROOD_SIGNAL_SETTING,
                 .hastened = true},
    TyrantAttack{.attack = TYRANT_ENRAGED_TENTACLE_ATTACK,
                 .signalSetting = TYRANT_TENTACLE_SIGNAL_SETTING,
                 .hastened = true},
    TyrantAttack{.attack = TYRANT_SWEEP_ATTACK,
                 .signalSetting = TYRANT_SWEEP_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_SPORES_ATTACK,
                 .signalSetting = TYRANT_SPORES_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_WALL_ATTACK,
                 .signalSetting = TYRANT_WALL_SIGNAL_SETTING},
    TyrantAttack{.attack = TYRANT_CHARGE_ATTACK,
                 .signalSetting = TYRANT_CHARGE_SIGNAL_SETTING}};

/** @brief The order it attacks in, calm then enraged, picked by the count of
 * the actions it ended: its arrival ends the first, so the calm Tyrant opens
 * with a burst. A ship within its melee reach takes the tentacles instead. */
constexpr std::array TYRANT_CALM_ROTATION{
    TYRANT_BROOD_ATTACK, TYRANT_BURST_ATTACK, TYRANT_BEAM_ATTACK,
    TYRANT_BURST_ATTACK};
constexpr std::array TYRANT_ENRAGED_ROTATION{
    TYRANT_CHARGE_ATTACK,       TYRANT_ENRAGED_BURST_ATTACK,
    TYRANT_SWEEP_ATTACK,        TYRANT_SPORES_ATTACK,
    TYRANT_ENRAGED_BEAM_ATTACK, TYRANT_WALL_ATTACK,
    TYRANT_ENRAGED_BROOD_ATTACK};

/** @brief Half its 512-unit frame: its center that far past an edge is
 * where the last of its body leaves the screen. */
constexpr float TYRANT_HALF_FRAME = 256.0F;
/** @brief How far past an edge its charge takes its center, in logical
 * units: TYRANT_HALF_FRAME and TYRANT_OFF_SCREEN_SLACK more, so it is wholly
 * off screen. */
constexpr float TYRANT_OFF_SCREEN_SLACK = 64.0F;
constexpr float TYRANT_OFF_SCREEN_MARGIN =
    TYRANT_HALF_FRAME + TYRANT_OFF_SCREEN_SLACK;
constexpr float TYRANT_CHARGE_LEFT_COLUMN = -TYRANT_OFF_SCREEN_MARGIN;
constexpr float TYRANT_CHARGE_RIGHT_COLUMN =
    PLAYFIELD_WIDTH + TYRANT_OFF_SCREEN_MARGIN;
/** @brief The column its center must be past before it leaps back to the
 * right: the last of its body left the screen. */
constexpr float TYRANT_HIDDEN_LEFT_COLUMN = -TYRANT_HALF_FRAME;
/** @brief The share of the strike of its charge spent going out on the left;
 * the rest brings it back from the right. */
constexpr float TYRANT_CHARGE_TURN_SHARE = 0.5F;
/** @brief The middle column of the playfield: once its charge column is past
 * it while the body is short of it, the charge has turned and the body leaps
 * there. */
constexpr float TYRANT_PLAYFIELD_MIDDLE_COLUMN = PLAYFIELD_WIDTH / 2.0F;

}  // namespace rtype::game
