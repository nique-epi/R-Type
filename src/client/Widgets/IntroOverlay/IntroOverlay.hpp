#pragma once

#include "IInterfaceCanvas.hpp"
#include "IntroOverlayLook.hpp"

namespace rtype::client {

/**
 * @brief Paints what lies over the scene of the intro at @p real seconds of
 * the music: the bands of the cinema format, which open at INTRO_SLAM; the
 * credit during the first beat, then the nickname of each pilot during its
 * close-up, typed on the bottom band and underlined in the color the palette
 * of @p look gives the pilot; the logo, which slams in from the right at
 * INTRO_SLAM to the place the title screen shows it at; and PRESS ANY KEY from
 * INTRO_HINT_START, on the bottom band while it shows, at the footer once it
 * opened. Over all of it, the ink of the black second before time 0, which
 * fades out in INTRO_FADE_IN_TIME.
 *
 * @param look The whole texture of the logo, and the palette of the pilots.
 */
void paintIntroOverlay(IInterfaceCanvas& canvas, float real,
                       IntroOverlayLook look);

}  // namespace rtype::client
