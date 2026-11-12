#pragma once

#include <array>
#include "NamedNumber.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

/** @brief The integer settings of the file: the volumes, written first,
 * those of the chat, then those of the lagometer. */
constexpr std::array VOLUME_NUMBERS{
    NamedNumber{.name = MUSIC_VOLUME_NAME, .setting = &MUSIC_VOLUME_SETTING},
    NamedNumber{.name = EFFECTS_VOLUME_NAME,
                .setting = &EFFECTS_VOLUME_SETTING},
};
constexpr std::array CHAT_NUMBERS{
    NamedNumber{.name = CHAT_LETTERS_NAME, .setting = &CHAT_LETTERS_SETTING},
    NamedNumber{.name = CHAT_CLOSED_LINES_NAME,
                .setting = &CHAT_CLOSED_LINES_SETTING},
    NamedNumber{.name = CHAT_OPEN_LINES_NAME,
                .setting = &CHAT_OPEN_LINES_SETTING},
    NamedNumber{.name = CHAT_SECONDS_NAME, .setting = &CHAT_SECONDS_SETTING},
    NamedNumber{.name = CHAT_BACKGROUND_NAME,
                .setting = &CHAT_BACKGROUND_SETTING},
};
constexpr std::array LAGOMETER_NUMBERS{
    NamedNumber{.name = LAGOMETER_SCALE_NAME,
                .setting = &LAGOMETER_SCALE_SETTING},
    NamedNumber{.name = LAGOMETER_BACKGROUND_NAME,
                .setting = &LAGOMETER_BACKGROUND_SETTING},
};

}  // namespace rtype::client
