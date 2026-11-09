"""Writes the update manifest of a release: its version, its notes, and the
client package of each system with its size and its SHA-256.

Usage: python3 -I packaging/update-manifest.py <version> <notes file>
<packages folder> <output file>

The notes are the lines of the tag message after its title, a leading "- "
removed; the client shows them in WHAT'S NEW, so their number and length are
bounded as the client bounds them (MAXIMUM_NOTE_LINES, MAXIMUM_NOTE_SIZE in
src/client/Update/Release/ReleaseConstants.hpp).
"""

import hashlib
import json
import pathlib
import sys

PACKAGES = {
    "Linux": "R-Type-client-{version}-Linux.AppImage",
    "Darwin": "R-Type-client-{version}-Darwin.dmg",
    "win64": "R-Type-client-{version}-win64.exe",
}
MAXIMUM_NOTE_LINES = 8
MAXIMUM_NOTE_SIZE = 48
BULLET = "- "
CHUNK_SIZE = 1 << 20


def notes_of(text):
    """Every line of the tag message after its title, its bullet removed."""
    lines = [line.strip() for line in text.splitlines()[1:]]
    notes = [line.removeprefix(BULLET) for line in lines if line]
    if len(notes) > MAXIMUM_NOTE_LINES:
        sys.exit(f"the tag message holds {len(notes)} notes, "
                 f"at most {MAXIMUM_NOTE_LINES}")
    for note in notes:
        if len(note) > MAXIMUM_NOTE_SIZE or not note.isascii() \
                or not note.isprintable():
            sys.exit(f"note '{note}': at most {MAXIMUM_NOTE_SIZE} "
                     "printable ASCII characters")
    return notes


def package_of(folder, name):
    """The size and the SHA-256 of one package."""
    path = folder / name
    digest = hashlib.sha256()
    with path.open("rb") as package:
        for chunk in iter(lambda: package.read(CHUNK_SIZE), b""):
            digest.update(chunk)
    return {"file": name, "size": path.stat().st_size,
            "sha256": digest.hexdigest()}


def main():
    version, notes_file, packages_folder, output = sys.argv[1:5]
    folder = pathlib.Path(packages_folder)
    manifest = {
        "version": version,
        "notes": notes_of(pathlib.Path(notes_file).read_text(encoding="utf-8")),
        "packages": {system: package_of(folder, name.format(version=version))
                     for system, name in PACKAGES.items()},
    }
    pathlib.Path(output).write_text(json.dumps(manifest, indent=2),
                                    encoding="utf-8")


if __name__ == "__main__":
    main()
