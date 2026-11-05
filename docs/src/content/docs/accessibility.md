---
title: Accessibility
description: What the game and its documentation offer for each category of disability, what is planned, what is missing, and which games the solutions draw from.
---

The project must be accessible to people with disabilities, its documentation included. This page lists, for each category the subject names, the needs the team retained, the solution, and its status. It also says what is still missing and which games the solutions draw from. How each solution works is on the [Client](/R-Type/client/) page.

A solution has one of two statuses:

| Status | Meaning |
|---|---|
| MVP | Part of the first delivery; it exists in the client today |
| V1 | Planned for the final delivery; not written yet |

The player finds the settings in the [options](/R-Type/client/#options-optionsscreen), which open from the title screen and, with Escape, during a game: the keys in the CONTROLS tab, the volumes in SOUND, the others in ACCESS. They are saved for the next launch.

## Physical and motor disabilities

| Need | Solution | Status |
|---|---|---|
| Play with a key layout that suits the hands | [Key remapping](/R-Type/client/#options-optionsscreen): the moves, SHOOT and READY each take another key. Two actions on one key are reported, and DEFAULT KEYS restores the layout | MVP |
| Play without holding or tapping a key for a whole game | [AUTO-FIRE](/R-Type/client/#player-commands-playercommands): the ship fires with no key held. The server still fires at most one missile per 333 ms (the gun of the `ship` sheet, three per second) per ship, so the option gives no advantage | MVP |
| Play without a keyboard | A gamepad for the moves, the shot and the menus, its buttons remappable like the keys | V1 |

## Audio and visual disabilities

| Need | Solution | Status |
|---|---|---|
| Lose nothing essential without the sound | [SOUND CUES](/R-Type/client/#sound-cues-soundcues): `!` over a Bydo that fires, and `P2 HIT` over a pilot hit | MVP |
| Adapt the sound to one's hearing | [Volumes](/R-Type/client/#options-optionsscreen): MUSIC and EFFECTS apart, from 0 to 100 %, and MUTE ALL | MVP |
| Tell the pilots apart with a color blindness | [COLOR MODE](/R-Type/client/#color-modes-playerpalette): a palette for protanopia, deuteranopia or tritanopia. The [status band](/R-Type/client/#status-band-scoreboard-paintstatusband) also gives each pilot a marker: triangle, square, circle or star | MVP |
| Tell the missiles apart without their color | A missile of a player is 16 × 6 pixels of the sheet, one of an enemy 8 × 8 | MVP |
| Play without flashes or shaking | [REDUCE FLASHES](/R-Type/client/#reduced-flashes-flashpreference): nothing flashes, shakes, recoils or vibrates, and an invulnerable ship is dithered all along and outlined. On a first launch it follows the "reduce motion" setting of the system | MVP |
| Be protected from large flashes with the option off | [`FlashBudget`](/R-Type/client/#flash-budget-and-camera-shake-flashbudget-camerashake) keeps the white on screen under `FLASH_AREA_MAXIMUM` (3600 pixels, 2.8 % of the screen), the share WCAG 2.3.1 sets | MVP |
| Read the interface with low vision | A larger interface text and a high contrast mode for the status band and the menus | V1 |
| Read the documentation with a screen reader or a keyboard | Every diagram has a text alternative, and a table or code block that scrolls takes the keyboard focus. An axe-core audit of the 11 pages the site then held, in both themes, reported no violation; later pages were not audited | MVP |

## Mental and cognitive disabilities

| Need | Solution | Status |
|---|---|---|
| Understand the game without reading a document | [Help of the waiting room](/R-Type/client/#help-of-the-waiting-room-helpwalkthrough): Professor Bydo explains the room and a moment of a game in 15 steps, one at a time, with the keys the player chose | MVP |
| Follow what the game asks at any moment | One word in the status band tells the state of each pilot (`LAST LIFE`, `RESPAWN`, `YOU`), and the options describe the selected row and what to do next | MVP |
| Have more time to react | An assisted mode: slower enemies and more lives, applied by the server to the whole game and shown to every player. It serves motor disabilities as well | V1 |

## What is not covered

These gaps exist in the client today.

| Gap | Effect on the player |
|---|---|
| The help opens from the waiting room only | It cannot be reached from the title screen or during a game, so a player who forgets a key mid-game has to look in the options |
| The arrival of a wave has no cue | A player who cannot hear misses it; the client receives no message when a wave starts, so this needs a change of the protocol |
| The four ships look the same during a game | The sheet holds four ship colors, but the game draws the first one for every pilot. Neither color nor shape tells the ships apart, and the color mode does not reach them |
| Escape and `HELP_KEY` (H) cannot be remapped | In the waiting room the HELP chip replaces H; during a game, Escape is the way to the options, so a player who cannot reach it cannot open them |
| Nothing is planned for players who cannot see the screen | The menus are not read aloud, and the game gives no audio equivalent of the position of what it shows |

## Games the solutions draw from

The subject asks to draw inspiration from features of actual games. Each game below offers the feature named next to it.

| Game | Its feature | What it gave here |
|---|---|---|
| [The Last of Us Part II](https://www.playstation.com/en-cz/games/the-last-of-us-part-ii/accessibility/) | Remaps every command; a larger status display; a high contrast display; camera shake and flashing of the interface adjustable | Key remapping, the planned text size and high contrast, REDUCE FLASHES |
| [Operation STEEL](https://leanderish.itch.io/steel/devlog/612421/v144) | An accessibility option that fires with no button held | AUTO-FIRE |
| [Overwatch](https://support.blizzard.com/de/article/31224) | A filter for protanopia, deuteranopia or tritanopia | COLOR MODE and its three forms, with one departure: [players criticized the filter](https://mein-mmo.de/en/overwatch-colorblind-mode,140594/) because it recolors the whole picture, so R-Type repaints the pilots only |
| [Fortnite](https://www.gamespot.com/articles/how-to-use-visualize-audio-effects-in-fortnite/1100-6498314/) | Visualize Sound Effects: sounds shown as indicators on screen | SOUND CUES |
| [Tales of Kenzera: ZAU](https://caniplaythat.com/2024/04/25/tales-of-kenzera-zau-launches-with-accessibility-portal/) | Controls to review in the menu, tutorials kept in a codex | The help |
| [Celeste](https://www.gamedeveloper.com/design/check-out-i-celeste-s-i-remarkably-granular-assist-options) | Assist Mode: a slower game, invincibility | The planned assisted mode |

The [Game Accessibility Guidelines](https://gameaccessibilityguidelines.com/) are the checklist: their basic level asks for remappable controls, no essential information by color or sound alone, separate volumes, and no flickering images; their intermediate level asks for a reminder of the controls during play and more than one input device. The site presents its levels as a way to set priorities, not as an audit, so this page claims no conformance level.
