#!/bin/sh
#
# Checks that the packages of the game work once they leave the repository.
#
# Usage: sh tests/tools/packaging/check-package.sh <folder holding the packages>
#
# The CI runs it right after CPack, on Linux, Windows and macOS. It answers
# three questions, and stops at the first "no":
#
#   1. Does each package hold its own program and files, and only those?
#      The client package must carry its sprites and must not carry the
#      server, nor the waves file only the server reads; the server package
#      must carry the waves and must not carry the client, nor the music only
#      the client plays. On Linux the AppImage is unpacked to check it too.
#
#   2. Does the client start?
#      It is run with --help. If a library it needs is missing from the
#      package (a DLL on Windows), it cannot even start. On macOS its
#      signature is checked too and, when RTYPE_EXPECT_NOTARIZED is 1,
#      Gatekeeper must accept the disk image and the app as notarized.
#
#   3. Does the server find its files from any folder?
#      It is started from an empty folder, far from the package. It stops at
#      once when it does not find assets/entities and assets/waves next to its
#      executable; so if it is still running after 3 seconds, it found them.

set -eu

# Made absolute, since the programs are run from another folder.
packages_folder=$(cd "$1" && pwd)
system=$(uname -s)

# On Windows the programs end in .exe; elsewhere they have no extension.
if [ "$system" = "Linux" ] || [ "$system" = "Darwin" ]; then
  exe=""
else
  exe=".exe"
fi

fail() {
  echo "FAILED: $1"
  exit 1
}

passed() {
  echo "OK: $1"
}

# Unpacks the archive of a program ("client" or "server") into a new temporary
# folder, then prints the folder that holds its bin/ and assets/.
unpack_archive() {
  program=$1
  destination=$(mktemp -d)
  if [ "$exe" = "" ]; then
    tar -xzf "$packages_folder"/R-Type-"$program"-*.tar.gz -C "$destination"
  else
    unzip -q "$packages_folder"/R-Type-"$program"-*.zip -d "$destination"
  fi
  echo "$destination"/R-Type-"$program"-*
}

# Opens the macOS disk image disk_image_file and prints the folder it is
# mounted on. The image is closed again when the script ends, whatever happens.
open_disk_image() {
  mount_point=$(mktemp -d)
  hdiutil attach -nobrowse -readonly -mountpoint "$mount_point" \
    "$disk_image_file" > /dev/null
  echo "$mount_point"
}

# Opens both packages and sets where each program and its files are:
# server_folder, server_program, client_folder, client_program.
open_packages() {
  echo "Opening the packages"
  server_folder=$(unpack_archive server)
  server_program=$server_folder/bin/r-type_server$exe

  if [ "$system" = "Darwin" ]; then
    # On macOS the client is an app: its program and its files sit inside
    # R-Type.app, in Contents/MacOS and Contents/Resources.
    disk_image_file=$(echo "$packages_folder"/R-Type-client-*.dmg)
    disk_image=$(open_disk_image)
    trap 'hdiutil detach "$disk_image" > /dev/null' EXIT
    client_folder=$disk_image/R-Type.app/Contents/Resources
    client_program=$disk_image/R-Type.app/Contents/MacOS/r-type_client
  else
    client_folder=$(unpack_archive client)
    client_program=$client_folder/bin/r-type_client$exe
  fi
}

# Question 1: does each package hold its own program and files, and only
# those?
check_each_package_holds_only_its_program() {
  echo "1. Each package holds its own program and files, and only those"
  [ -d "$client_folder/assets/sprites" ] ||
    fail "the client package has no assets/sprites"
  [ -d "$server_folder/assets/waves" ] ||
    fail "the server package has no assets/waves"
  [ -d "$server_folder/assets/entities" ] ||
    fail "the server package has no assets/entities"
  [ -d "$server_folder/assets/sprites" ] ||
    fail "the server package has no assets/sprites, which its content pack serves"
  [ ! -e "$(dirname "$client_program")/r-type_server$exe" ] ||
    fail "the client package also holds the server"
  [ ! -e "$client_folder/assets/waves" ] ||
    fail "the client package holds the waves file of the server"
  [ ! -e "$server_folder/bin/r-type_client$exe" ] ||
    fail "the server package also holds the client"
  [ ! -e "$server_folder/assets/music" ] ||
    fail "the server package holds the music of the client"
  passed "the client and the server are packaged apart, each with its files"

  if [ "$system" = "Linux" ]; then
    appimage=$(echo "$packages_folder"/R-Type-client-*.AppImage)
    "$appimage" --appimage-extract > /dev/null
    [ -d squashfs-root/usr/assets/sprites ] ||
      fail "the AppImage has no usr/assets/sprites"
    passed "the AppImage holds the files of the client"
  fi

  if [ "$system" = "Darwin" ]; then
    [ -f "$disk_image/.background/background.tiff" ] ||
      fail "the disk image has no background"
    [ -f "$disk_image/.DS_Store" ] ||
      fail "the disk image has no .DS_Store to lay out its window"
    [ -L "$disk_image/Applications" ] ||
      fail "the disk image has no link to Applications"
    passed "the disk image holds its background, its layout and Applications"
  fi
}

# Question 2: does the client start? On Linux the AppImage too; on macOS the
# app must also carry a valid signature.
check_the_client_starts() {
  echo "2. The client starts"
  "$client_program" --help > /dev/null || fail "the client does not start"
  passed "the client starts"

  if [ "$system" = "Linux" ]; then
    appimage=$(echo "$packages_folder"/R-Type-client-*.AppImage)
    "$appimage" --help > /dev/null || fail "the AppImage does not start"
    passed "the AppImage starts"
  fi

  if [ "$system" = "Darwin" ]; then
    codesign --verify --strict "$disk_image/R-Type.app" ||
      fail "the signature of R-Type.app is not valid"
    passed "R-Type.app is signed"
    if [ "${RTYPE_EXPECT_NOTARIZED:-0}" = "1" ]; then
      check_the_app_is_notarized
    fi
  fi
}

# Gatekeeper accepts the disk image and the app only when Apple notarized them,
# and the ticket stapled to the image lets it decide without a connection.
check_the_app_is_notarized() {
  xcrun stapler validate "$disk_image_file" > /dev/null ||
    fail "the disk image carries no notarization ticket"
  spctl --assess --type open --context context:primary-signature \
    "$disk_image_file" 2> /dev/null ||
    fail "Gatekeeper refuses the disk image"
  spctl --assess --type execute "$disk_image/R-Type.app" 2> /dev/null ||
    fail "Gatekeeper refuses R-Type.app"
  passed "R-Type.app is notarized and Gatekeeper accepts it"
}

# Question 3: does the server find its files from any folder? It stops at once
# without assets/entities or assets/waves, so still running after 3 seconds
# means it found them.
check_the_server_finds_its_files() {
  echo "3. The server finds its files from any folder"
  "$server_program" > /dev/null 2>&1 &
  server_process=$!
  sleep 3
  kill -0 "$server_process" 2> /dev/null ||
    fail "the server stopped: it did not find its assets next to it"
  kill "$server_process"
  passed "the server is still running after 3 seconds"
}

open_packages
# Every program runs from an empty folder, so nothing next to the repository
# can help it; the AppImage is unpacked there too.
cd "$(mktemp -d)"
check_each_package_holds_only_its_program
check_the_client_starts
check_the_server_finds_its_files
echo "All packages checked"
