#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "LanguageConstants.hpp"
#include "SettingsConstants.hpp"
#include "Slider.hpp"
#include "SliderLook.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

/** @brief Heading of the options, and the notice written over a game. */
constexpr Translation SETTINGS_HEADING{.english = "SETTINGS",
                                       .french = "OPTIONS"};
constexpr Translation GAME_GOES_ON_NOTICE{.english = "THE GAME GOES ON",
                                          .french = "LA PARTIE CONTINUE"};

/** @brief The tabs, left to right, and the order of each. */
constexpr std::size_t OPTIONS_TAB_COUNT = 5;
constexpr std::array<Translation, OPTIONS_TAB_COUNT> OPTIONS_TAB_LABELS{
    Translation{.english = "SOUND", .french = "SON"},
    Translation{.english = "CONTROLS", .french = "TOUCHES"},
    Translation{.english = "DISPLAY", .french = "AFFICHAGE"},
    Translation{.english = "ACCESS", .french = "ACCÈS"},
    Translation{.english = "CHAT", .french = "CHAT"}};
constexpr std::size_t SOUND_TAB = 0;
constexpr std::size_t CONTROLS_TAB = 1;
constexpr std::size_t DISPLAY_TAB = 2;
constexpr std::size_t ACCESS_TAB = 3;
constexpr std::size_t CHAT_TAB = 4;

/** @brief Order of the rows whose description changes, in their tab. */
constexpr std::size_t MUSIC_ROW = 0;
constexpr std::size_t EFFECTS_ROW = 1;
constexpr std::size_t VOICES_ROW = 2;
constexpr std::size_t FIRST_KEY_ROW = 0;
constexpr std::size_t DISPLAY_MODE_ROW = 0;
constexpr std::size_t WINDOW_SIZE_ROW = 1;
constexpr std::size_t CHAT_KEY_ROW = 1;

/** @brief CONTROLS lists the key of every bound action but the last, OPEN
 * CHAT, which CHAT lists: CONTROLS already holds the eight rows of a panel. */
constexpr std::size_t OPEN_CHAT_ACTION =
    static_cast<std::size_t>(BoundAction::OpenChat);
static_assert(OPEN_CHAT_ACTION == BOUND_ACTION_COUNT - 1,
              "OPEN CHAT is the last bound action");

/** @brief How a description starts. */
constexpr std::string_view DESCRIPTION_START = "> ";

/**
 * @brief SOUND: the volumes, a percent of their range, moved by
 * VOLUME_STEP, their description built around their default; muting all;
 * putting the volumes back.
 */
constexpr Slider::Range VOLUME_RANGE{.minimum = MINIMUM_VOLUME,
                                     .maximum = MAXIMUM_VOLUME,
                                     .step = VOLUME_STEP,
                                     .pageSteps = VOLUME_PAGE_STEPS};
constexpr const char* PERCENT_UNIT = "%";
constexpr Translation MUSIC_LABEL{.english = "MUSIC", .french = "MUSIQUE"};
constexpr Translation EFFECTS_LABEL{.english = "EFFECTS", .french = "EFFETS"};
constexpr Translation VOICES_LABEL{.english = "VOICES", .french = "VOIX"};
constexpr std::string_view STEP_MARK = "{STEP}";
constexpr std::string_view DEFAULT_MARK = "{DEFAULT}";
constexpr Translation VOLUME_DESCRIPTION{
    .english = "> BY {STEP} %, DEFAULT {DEFAULT} %",
    .french = "> PAR {STEP} %, DÉFAUT {DEFAULT} %"};
constexpr Translation MUTED_VOLUME_DESCRIPTION{
    .english = "> SOUND MUTED: SEE MUTE ALL",
    .french = "> SON COUPÉ : VOIR TOUT COUPER"};
constexpr Translation MUTE_ALL_LABEL{.english = "MUTE ALL",
                                     .french = "TOUT COUPER"};
constexpr Translation MUTE_ALL_DESCRIPTION{.english = "> SILENCES ALL",
                                           .french = "> COUPE TOUS LES SONS"};
constexpr Translation DEFAULT_SOUND_LABEL{.english = "DEFAULT SOUND",
                                          .french = "SON PAR DÉFAUT"};
constexpr Translation DEFAULT_SOUND_DESCRIPTION{
    .english = "> PUTS BACK THE VOLUMES OF A FIRST LAUNCH",
    .french = "> REMET LES VOLUMES D'ORIGINE"};
constexpr Translation DEFAULT_SOUND_QUESTION{
    .english = "PUT BACK THE DEFAULT SOUND?",
    .french = "REMETTRE LE SON PAR DÉFAUT ?"};
constexpr Translation RESET_TEXT{.english = "RESET", .french = "RÉTABLIR"};

/** @brief CONTROLS: the label of each bound action, in the order of
 * BOUND_ACTIONS; what a key row says; putting the keys back. */
constexpr std::array<Translation, BOUND_ACTION_COUNT> BOUND_ACTION_LABELS{
    Translation{.english = "MOVE UP", .french = "MONTER"},
    Translation{.english = "MOVE DOWN", .french = "DESCENDRE"},
    Translation{.english = "MOVE LEFT", .french = "GAUCHE"},
    Translation{.english = "MOVE RIGHT", .french = "DROITE"},
    Translation{.english = "SHOOT", .french = "TIRER"},
    Translation{.english = "READY", .french = "PRÊT"},
    Translation{.english = "OPEN CHAT", .french = "OUVRIR LE CHAT"}};
constexpr Translation KEY_DESCRIPTION{
    .english = "> ENTER, THEN THE NEW KEY",
    .french = "> ENTRÉE, PUIS LA NOUVELLE TOUCHE"};
constexpr Translation LISTENING_DESCRIPTION{
    .english = "> PRESS THE NEW KEY. ESC CANCELS",
    .french = "> APPUYEZ SUR LA TOUCHE. ÉCHAP ANNULE"};
constexpr Translation REFUSED_KEY_DESCRIPTION{
    .english = "> THIS KEY CANNOT BE BOUND",
    .french = "> CETTE TOUCHE EST INTERDITE"};
constexpr std::string_view ACTION_MARK = "{ACTION}";
constexpr Translation CONFLICT_DESCRIPTION{
    .english = "> SAME KEY AS {ACTION}: CHANGE ONE",
    .french = "> DÉJÀ PRISE PAR {ACTION}"};
constexpr Translation DEFAULT_KEYS_LABEL{.english = "DEFAULT KEYS",
                                         .french = "TOUCHES D'ORIGINE"};
constexpr Translation DEFAULT_KEYS_DESCRIPTION{
    .english = "> PUTS BACK THE KEYS OF A FIRST LAUNCH",
    .french = "> REMET LES TOUCHES D'ORIGINE"};
constexpr Translation DEFAULT_KEYS_QUESTION{
    .english = "PUT BACK THE DEFAULT KEYS?",
    .french = "REMETTRE LES TOUCHES D'ORIGINE ?"};

/** @brief DISPLAY: the display mode, its choices in the order of
 * DISPLAY_MODES, the size of the window, and the language, its choices in the
 * order of LANGUAGES, each written in its own language so a player finds
 * theirs without reading the other; and what their rows say. */
constexpr Translation LANGUAGE_LABEL{.english = "LANGUAGE", .french = "LANGUE"};
constexpr Translation LANGUAGE_DESCRIPTION{
    .english = "> THE LANGUAGE OF EVERY TEXT",
    .french = "> LA LANGUE DE TOUS LES TEXTES"};
constexpr std::array<std::string_view, LANGUAGE_COUNT> LANGUAGE_CHOICE_LABELS{
    "ENGLISH", "FRANÇAIS"};
constexpr Translation DISPLAY_MODE_LABEL{.english = "DISPLAY MODE",
                                         .french = "AFFICHAGE"};
constexpr std::array<Translation, DISPLAY_MODE_COUNT> DISPLAY_MODE_LABELS{
    Translation{.english = "WINDOWED", .french = "FENÊTRÉ"},
    Translation{.english = "BORDERLESS", .french = "SANS BORDURE"}};
constexpr Translation DISPLAY_MODE_DESCRIPTION{
    .english = "> ENTER OPENS THE LIST OF MODES",
    .french = "> ENTRÉE OUVRE LA LISTE DES MODES"};
constexpr Translation BORDERLESS_DESCRIPTION{
    .english = "> BORDERLESS COVERS THE WHOLE SCREEN",
    .french = "> SANS BORDURE COUVRE TOUT L'ÉCRAN"};
constexpr Translation WINDOW_SIZE_LABEL{.english = "WINDOW SIZE",
                                        .french = "TAILLE"};
constexpr Translation WINDOW_SIZE_DESCRIPTION{
    .english = "> ENTER OPENS THE LIST OF SIZES",
    .french = "> ENTRÉE OUVRE LA LISTE DES TAILLES"};
constexpr Translation OPEN_LIST_DESCRIPTION{
    .english = "> UP / DOWN, THEN ENTER. ESC CLOSES",
    .french = "> HAUT / BAS, PUIS ENTRÉE. ÉCHAP FERME"};
constexpr Translation POINTED_TOO_LARGE_END{
    .english = ": TOO LARGE FOR THIS SCREEN",
    .french = " : TROP GRANDE POUR CET ÉCRAN"};
constexpr Translation LARGER_TOO_LARGE_END{.english = " IS TOO LARGE HERE",
                                           .french = " EST TROP GRANDE ICI"};

/** @brief ACCESS: the color modes, in the order of COLOR_MODES, and the
 * other rows. */
constexpr Translation COLOR_MODE_LABEL{.english = "COLOR MODE",
                                       .french = "MODE COULEUR"};
constexpr Translation COLOR_MODE_DESCRIPTION{
    .english = "> PILOT COLORS TOLD APART",
    .french = "> COULEURS DES PILOTES DISTINCTES"};
constexpr std::array<Translation, COLOR_MODE_COUNT> COLOR_MODE_LABELS{
    Translation{.english = "OFF", .french = "AUCUN"},
    Translation{.english = "PROTANOPIA", .french = "PROTANOPIE"},
    Translation{.english = "DEUTERANOPIA", .french = "DEUTÉRANOPIE"},
    Translation{.english = "TRITANOPIA", .french = "TRITANOPIE"}};
constexpr Translation REDUCE_FLASHES_LABEL{.english = "REDUCE FLASHES",
                                           .french = "MOINS DE FLASHS"};
constexpr Translation REDUCE_FLASHES_DESCRIPTION{
    .english = "> NO FLASH OR SHAKE", .french = "> NI FLASH NI SECOUSSE"};
constexpr Translation SOUND_CUES_LABEL{.english = "SOUND CUES",
                                       .french = "SONS ÉCRITS"};
constexpr Translation SOUND_CUES_DESCRIPTION{
    .english = "> SOUNDS SHOWN ON SCREEN",
    .french = "> LES SONS AFFICHÉS À L'ÉCRAN"};
constexpr Translation AUTO_FIRE_LABEL{.english = "AUTO-FIRE",
                                      .french = "TIR AUTO"};
constexpr Translation AUTO_FIRE_DESCRIPTION{
    .english = "> SHOOTS WITHOUT HOLDING", .french = "> TIRE SANS MAINTENIR"};
constexpr Translation PILOT_NAMES_LABEL{.english = "PILOT NAMES",
                                        .french = "NOMS PILOTES"};
constexpr Translation PILOT_NAMES_DESCRIPTION{
    .english = "> NICKNAMES OVER THE SHIPS",
    .french = "> PSEUDOS AU-DESSUS DES VAISSEAUX"};
constexpr Translation LAGOMETER_SCALE_LABEL{.english = "LAGOMETER SIZE",
                                            .french = "LAGOMÈTRE"};
constexpr Translation LAGOMETER_SCALE_DESCRIPTION{
    .english = "> F3 SHOWS IT IN GAME", .french = "> F3 L'AFFICHE EN JEU"};
constexpr Translation LAGOMETER_BACKGROUND_LABEL{.english = "LAGOMETER FILL",
                                                 .french = "FOND LAGOMÈTRE"};
constexpr Translation LAGOMETER_BACKGROUND_DESCRIPTION{
    .english = "> DARKENS THE GAME BEHIND IT",
    .french = "> ASSOMBRIT LE JEU DERRIÈRE"};
constexpr const char* TIMES_UNIT = "X";
constexpr SliderLook LAGOMETER_SCALE_LOOK{.unit = TIMES_UNIT};
constexpr SliderLook LAGOMETER_BACKGROUND_LOOK{
    .step = LAGOMETER_BACKGROUND_STEP, .unit = PERCENT_UNIT};

/** @brief CHAT: its rows, what they say, and how each slider reads. */
constexpr Translation CHAT_SHOWN_LABEL{.english = "CHAT", .french = "CHAT"};
constexpr Translation CHAT_SHOWN_DESCRIPTION{
    .english = "> LINES OF THE OTHER PILOTS",
    .french = "> LES LIGNES DES AUTRES PILOTES"};
constexpr Translation CHAT_LETTERS_LABEL{.english = "LINE WIDTH",
                                         .french = "LARGEUR"};
constexpr Translation CHAT_LETTERS_DESCRIPTION{
    .english = "> LETTERS IN ONE LINE", .french = "> LETTRES PAR LIGNE"};
constexpr Translation CHAT_CLOSED_LINES_LABEL{.english = "LINES CLOSED",
                                              .french = "LIGNES EN JEU"};
constexpr Translation CHAT_CLOSED_LINES_DESCRIPTION{
    .english = "> LINES SHOWN WHILE YOU PLAY",
    .french = "> LIGNES VISIBLES PENDANT LE JEU"};
constexpr Translation CHAT_OPEN_LINES_LABEL{.english = "LINES OPEN",
                                            .french = "LIGNES OUVERT"};
constexpr Translation CHAT_OPEN_LINES_DESCRIPTION{
    .english = "> LINES SHOWN WHILE YOU TYPE",
    .french = "> LIGNES VISIBLES EN ÉCRIVANT"};
constexpr Translation CHAT_SECONDS_LABEL{.english = "SHOWN FOR",
                                         .french = "DURÉE"};
constexpr Translation CHAT_SECONDS_DESCRIPTION{
    .english = "> SECONDS A LINE STAYS WHILE YOU PLAY",
    .french = "> SECONDES D'UNE LIGNE EN JEU"};
constexpr Translation CHAT_BACKGROUND_LABEL{.english = "BACKGROUND",
                                            .french = "FOND"};
constexpr Translation CHAT_BACKGROUND_DESCRIPTION{
    .english = "> DARKENS THE GAME WHILE YOU TYPE",
    .french = "> ASSOMBRIT LE JEU EN ÉCRIVANT"};
constexpr const char* SECONDS_UNIT = "S";
constexpr SliderLook CHAT_LETTERS_LOOK{.step = CHAT_LETTERS_STEP};
constexpr SliderLook CHAT_LINES_LOOK{};
constexpr SliderLook CHAT_SECONDS_LOOK{.unit = SECONDS_UNIT};
constexpr SliderLook CHAT_BACKGROUND_LOOK{.step = CHAT_BACKGROUND_STEP,
                                          .unit = PERCENT_UNIT};

/** @brief BACK, and the stamp written after a save, SAVED for
 * SAVED_STAMP_DURATION, NOT SAVED until the next save. */
constexpr Translation BACK_DESCRIPTION{
    .english = "> CLOSES THE OPTIONS. ESC DOES TOO",
    .french = "> FERME LES OPTIONS. ÉCHAP AUSSI"};

/** @brief Description of QUIT, on the options over a game. */
constexpr Translation QUIT_GAME_DESCRIPTION{.english = "> LEAVES THE GAME",
                                            .french = "> QUITTE LA PARTIE"};
constexpr Translation SAVED_STAMP{.english = "SAVED", .french = "ENREGISTRÉ"};
constexpr Translation NOT_SAVED_STAMP{.english = "NOT SAVED",
                                      .french = "NON ENREGISTRÉ"};
constexpr engine::Duration SAVED_STAMP_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client
