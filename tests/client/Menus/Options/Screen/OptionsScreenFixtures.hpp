#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "RecordingInterfaceCanvas.hpp"

/** @returns The name of a key as the settings file writes it, as the
 * options would on a US keyboard. */
[[nodiscard]] std::string nameInTheFile(sf::Keyboard::Scancode key);

/** @returns The description of the selected row: the line written from
 * DESCRIPTION_LEFT that starts with "> ", or nullptr. */
[[nodiscard]] const RecordedWriting* descriptionOf(
    const InterfaceCanvasRecord& record);
