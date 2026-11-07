/** What each key does, by its `KeyboardEvent.key` in lower case: `q`, `escape`, `arrowright`. */
export type Shortcuts = Readonly<Record<string, (() => void) | undefined>>;

/**
 * Runs the action of the key pressed, if it has one, and keeps the browser
 * from doing anything else with that key. A letter acts whether Shift or
 * Caps Lock is on. Returns whether a shortcut ran.
 */
export function runShortcut(
  event: Pick<KeyboardEvent, 'key' | 'preventDefault'>,
  shortcuts: Shortcuts,
): boolean {
  const action = shortcuts[event.key.toLowerCase()];
  if (action === undefined) {
    return false;
  }
  event.preventDefault();
  action();
  return true;
}
