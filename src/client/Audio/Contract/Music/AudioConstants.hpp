#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "MusicTrack.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Most sounds the client plays at once, for the whole client: the
 * cascade of the intro starts 32 sounds in less than half a second, a game
 * a shot and an explosion per kill. A sound started while every voice plays
 * cuts the one that started first.
 */
constexpr std::size_t SOUND_VOICE_LIMIT = 16;

/**
 * @brief Asset id of the theme, played in a loop outside a game: Spacesuit
 * Suite, by Stationary Sign (see assets/README.md).
 */
constexpr const char* THEME_MUSIC_ID = "music/spacesuit_suite.mp3";

/**
 * @brief Where the first beat of the theme falls in its file: 85 ms in,
 * measured on its sixteenth notes (aubio onsets, within 82 to 88 ms in 17
 * windows of 10 s out of 20). The music counts its position from there, and
 * loops from there.
 */
constexpr engine::Duration THEME_FIRST_BEAT = std::chrono::milliseconds(85);

/**
 * @brief One beat of the theme, which plays 109 beats a minute, in seconds.
 * The theme loops over THEME_LOOP_BEATS beats from its first beat, 22
 * phrases of 16 beats that end 193.85 s into the file, before its fade out,
 * which starts at 197 s.
 */
constexpr float THEME_BEAT = 60.0F / 109.0F;
constexpr int THEME_LOOP_BEATS = 352;

/** @brief The theme as the music player plays it: looping over
 * THEME_LOOP_BEATS beats from its first beat, its position counted from that
 * beat. */
constexpr MusicTrack THEME_TRACK{
    .assetId = THEME_MUSIC_ID,
    .loopStart = THEME_FIRST_BEAT,
    .loopLength = std::chrono::duration_cast<engine::Duration>(
        std::chrono::duration<float>(THEME_BEAT *
                                     static_cast<float>(THEME_LOOP_BEATS))),
    .firstBeat = THEME_FIRST_BEAT};

/**
 * @brief Asset id of the music of a game: Oh the City, by Machine Dreams (see
 * assets/README.md), 160.2 s, which starts at once. It loops from its start
 * to GAME_MUSIC_LOOP_END, before its fade out, which starts at 152.1 s
 * (measured on its level by tenth of a second).
 */
constexpr const char* GAME_MUSIC_ID = "music/oh_the_city.mp3";
constexpr engine::Duration GAME_MUSIC_LOOP_END = std::chrono::seconds(152);
constexpr MusicTrack GAME_TRACK{.assetId = GAME_MUSIC_ID,
                                .loopLength = GAME_MUSIC_LOOP_END};

/** @brief When a game starts, the theme stops at once and the music of the
 * game starts GAME_MUSIC_SILENCE later. */
constexpr engine::Duration GAME_MUSIC_SILENCE = std::chrono::seconds(1);

/**
 * @brief Asset id of the music of the Bydo Tyrant, played in a loop from the
 * end of its alarm until the game ends, faded in over the last
 * BOSS_MUSIC_FADE_IN of the alarm. It carries the screen like the music of a
 * game, at the full MUSIC volume.
 */
constexpr const char* BOSS_MUSIC_ID = "music/boss_theme.mp3";
constexpr MusicTrack BOSS_TRACK{.assetId = BOSS_MUSIC_ID};

/** @brief How long the alarm of the boss plays (8.232 s): the music of the
 * boss reaches its full volume when it ends. */
constexpr engine::Duration BOSS_ALARM_DURATION =
    std::chrono::milliseconds(8232);

/** @brief The fade level of a music that is not heard, and of one at the
 * volume of the settings. */
constexpr float MUSIC_FADE_SILENT = 0.0F;
constexpr float MUSIC_FADE_FULL = 1.0F;

/**
 * @brief How long the music that follows the alarm of a boss takes to rise
 * from silence to its full volume (3 s): it starts BOSS_MUSIC_FADE_IN before
 * the alarm ends, over its tail, which has fallen under -44 LUFS by then,
 * and reaches its full volume as the alarm ends.
 */
constexpr engine::Duration BOSS_MUSIC_FADE_IN = std::chrono::seconds(3);

/**
 * @brief The ambiences, played in a loop over the whole file under the theme:
 * deep space on the server map, a garage in the hangar. They only color the
 * theme, so they play at 0.25 of the MUSIC volume on the map, and in the hangar
 * the garage at 0.40 with a crowd talking under it at 0.50 (its file is louder,
 * so it is heard a little under the garage).
 */
constexpr float SPACE_AMBIENCE_GAIN = 0.25F;
constexpr float HANGAR_AMBIENCE_GAIN = 0.40F;
constexpr float CROWD_AMBIENCE_GAIN = 0.50F;
constexpr MusicTrack SPACE_AMBIENCE_TRACK{.assetId = "music/space_ambience.mp3",
                                          .gain = SPACE_AMBIENCE_GAIN};
constexpr MusicTrack HANGAR_AMBIENCE_TRACK{
    .assetId = "music/hangar_ambience.mp3", .gain = HANGAR_AMBIENCE_GAIN};
constexpr MusicTrack CROWD_AMBIENCE_TRACK{.assetId = "music/hangar_crowd.flac",
                                          .gain = CROWD_AMBIENCE_GAIN};

/** @brief The music of the shop, and the two musics its radio plays
 * instead once tuned, one then the other; each loops over its whole file,
 * at the MUSIC volume, while the theme is paused. */
constexpr MusicTrack SHOP_MUSIC_TRACK{.assetId =
                                          "music/hitchhiking_in_moscow.mp3"};
constexpr MusicTrack SHOP_RADIO_TRACK{.assetId =
                                          "music/papers_please_theme.mp3"};
constexpr MusicTrack SHOP_DANCE_TRACK{.assetId = "music/kavorka.mp3"};

/**
 * @brief The musics of the casino, each in a loop over the whole file while
 * the theme is paused: Move to the Music, by Wanda Shakes, before its doors;
 * To Hell and Back, by Elliot Holmes, in its hall, over a stadium crowd at
 * CASINO_CROWD_GAIN of the MUSIC volume, the crowd of its boxing ring; and
 * American Swing, from the radio of Fallout: New Vegas, at the roulette and
 * at the cashier.
 */
constexpr float CASINO_CROWD_GAIN = 0.45F;
constexpr MusicTrack CASINO_FRONT_TRACK{.assetId =
                                            "music/move_to_the_music.mp3"};
constexpr MusicTrack CASINO_HALL_TRACK{.assetId = "music/to_hell_and_back.mp3"};
constexpr MusicTrack CASINO_CROWD_TRACK{.assetId = "music/boxing_crowd.mp3",
                                        .gain = CASINO_CROWD_GAIN};
constexpr MusicTrack CASINO_TABLES_TRACK{.assetId = "music/american_swing.mp3"};

/**
 * @brief How the time of the intro follows the position of the music, which
 * moves by one audio block at a time (about 10 ms in SFML 3.0.2): each frame
 * closes elapsed / MUSIC_CATCH_UP_TIME of the gap, a tenth of it for a frame
 * of 16 ms, so the steps of that position barely move the pictures and a gap
 * of 40 ms falls under 4 ms in about 0.37 s. A gap wider than MUSIC_SYNC_SNAP,
 * either way, is closed at once. MUSIC_SYNC_SNAP is shorter than
 * MUSIC_CATCH_UP_TIME, so a frame that closes part of a gap never moves the
 * time back.
 */
constexpr engine::Duration MUSIC_CATCH_UP_TIME = std::chrono::milliseconds(160);
constexpr engine::Duration MUSIC_SYNC_SNAP = std::chrono::milliseconds(100);

/** @brief Logger name the music and the sounds write their log lines under. */
constexpr std::string_view AUDIO_LOGGER_NAME = "Audio";

}  // namespace rtype::client
