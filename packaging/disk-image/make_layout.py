"""Draws the background of the client disk image and writes its .DS_Store."""

import os
import random
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont
from ds_store import DSStore
from mac_alias import Alias

FOLDER = Path(__file__).resolve().parent
REPOSITORY = FOLDER.parent.parent
VOLUME_NAME = "R-Type"
WINDOW_WIDTH = 600
WINDOW_HEIGHT = 400
WINDOW_LEFT = 200
WINDOW_TOP = 120
ICON_SIZE = 112
TEXT_SIZE = 12
APP_CENTER = (160, 196)
APPLICATIONS_CENTER = (440, 196)
RETINA_SCALE = 2
STAR_COUNT = 90
STAR_SEED = 1987
LOGO_SCALE = 2
LOGO_TOP = 40
CAPTION_TOP = 340
CAPTION = "DRAG R-TYPE INTO APPLICATIONS"
CAPTION_FONT_SIZE = 8
ARROW_HEAD = 10
ARROW_GAP = 12
ICON_DRAWN_TOP_OFFSET = -45
LABEL_BOTTOM_OFFSET = 81
BOX_MARGIN = 12
BOX_HALF = 64
BOX_CORNER = 6
BOX_TOP_OFFSET = ICON_DRAWN_TOP_OFFSET - BOX_MARGIN
BOX_BOTTOM_OFFSET = LABEL_BOTTOM_OFFSET + BOX_MARGIN

SKY_COLOR = (11, 13, 31)
DEEP_BLUE = (38, 48, 94)
PALE_BLUE = (143, 162, 230)
STAR_WHITE = (238, 241, 255)
STAR_GOLD = (255, 211, 138)
EDGE_BLUE = (31, 163, 201)
HUD_CYAN = (127, 230, 255)

STAR_COLORS = (DEEP_BLUE, DEEP_BLUE, PALE_BLUE, STAR_WHITE, STAR_GOLD)
BACKGROUND_NAME = "background.tiff"
DS_STORE_NAME = "DS_Store"


def paint_stars(draw):
    """Scatters the same stars on every run, away from the logo and the boxes."""
    stars = random.Random(STAR_SEED)
    for _ in range(STAR_COUNT):
        x = stars.randrange(WINDOW_WIDTH)
        y = stars.randrange(WINDOW_HEIGHT)
        draw.point((x, y), fill=stars.choice(STAR_COLORS))


def paint_logo(canvas):
    """Lays the logo of the game, twice its size, centred at the top."""
    logo = Image.open(REPOSITORY / "assets" / "sprites" / "logo.png").convert("RGBA")
    logo = logo.resize(
        (logo.width * LOGO_SCALE, logo.height * LOGO_SCALE), Image.NEAREST
    )
    canvas.alpha_composite(logo, ((WINDOW_WIDTH - logo.width) // 2, LOGO_TOP))


def paint_box(draw, center):
    """Draws a box with cut corners around the place of one icon."""
    left = center[0] - BOX_HALF
    right = center[0] + BOX_HALF
    top = center[1] + BOX_TOP_OFFSET
    bottom = center[1] + BOX_BOTTOM_OFFSET
    cut = BOX_CORNER
    draw.rectangle((left + cut, top, right - cut, bottom), fill=SKY_COLOR)
    draw.rectangle((left, top + cut, right, bottom - cut), fill=SKY_COLOR)
    draw.line(
        [
            (left + cut, top),
            (right - cut, top),
            (right, top + cut),
            (right, bottom - cut),
            (right - cut, bottom),
            (left + cut, bottom),
            (left, bottom - cut),
            (left, top + cut),
            (left + cut, top),
        ],
        fill=EDGE_BLUE,
    )


def paint_arrow(draw):
    """Draws the arrow from the app to the Applications folder."""
    y = APP_CENTER[1] + (BOX_TOP_OFFSET + BOX_BOTTOM_OFFSET) // 2
    arrow_left = APP_CENTER[0] + BOX_HALF + ARROW_GAP
    arrow_right = APPLICATIONS_CENTER[0] - BOX_HALF - ARROW_GAP
    draw.rectangle((arrow_left, y - 1, arrow_right - ARROW_HEAD, y + 1), fill=HUD_CYAN)
    for step in range(ARROW_HEAD + 1):
        x = arrow_right - ARROW_HEAD + step
        reach = ARROW_HEAD - step
        draw.line([(x, y - reach), (x, y + reach)], fill=HUD_CYAN)


def paint_caption(draw):
    """Writes what to do, centred under the boxes."""
    font = ImageFont.truetype(
        str(REPOSITORY / "assets" / "fonts" / "press_start_2p" / "press_start_2p.ttf"),
        CAPTION_FONT_SIZE,
    )
    width = draw.textlength(CAPTION, font=font)
    draw.text(
        ((WINDOW_WIDTH - width) // 2, CAPTION_TOP),
        CAPTION,
        font=font,
        fill=HUD_CYAN,
    )


def paint_background():
    """Returns the background at the size of the window, one pixel per point."""
    canvas = Image.new("RGBA", (WINDOW_WIDTH, WINDOW_HEIGHT), SKY_COLOR + (255,))
    draw = ImageDraw.Draw(canvas)
    draw.fontmode = "1"
    paint_stars(draw)
    paint_logo(canvas)
    paint_box(draw, APP_CENTER)
    paint_box(draw, APPLICATIONS_CENTER)
    paint_arrow(draw)
    paint_caption(draw)
    return canvas.convert("RGB")


def write_background(work):
    """Writes the background as a TIFF holding its normal and Retina sizes."""
    normal = paint_background()
    retina = normal.resize(
        (WINDOW_WIDTH * RETINA_SCALE, WINDOW_HEIGHT * RETINA_SCALE), Image.NEAREST
    )
    normal_path = work / "background.png"
    retina_path = work / "background@2x.png"
    normal.save(normal_path, dpi=(72, 72))
    retina.save(retina_path, dpi=(144, 144))
    subprocess.run(
        [
            "tiffutil",
            "-cathidpicheck",
            str(normal_path),
            str(retina_path),
            "-out",
            str(FOLDER / BACKGROUND_NAME),
        ],
        check=True,
    )


def window_settings():
    """Returns the Finder settings of the window: size, place, no toolbar."""
    return {
        "ShowStatusBar": False,
        "WindowBounds": (
            f"{{{{{WINDOW_LEFT}, {WINDOW_TOP}}}, {{{WINDOW_WIDTH}, {WINDOW_HEIGHT}}}}}"
        ),
        "ContainerShowSidebar": False,
        "PreviewPaneVisibility": False,
        "SidebarWidth": 0,
        "ShowTabView": False,
        "ShowToolbar": False,
        "ShowPathbar": False,
        "ShowSidebar": False,
    }


def icon_settings(background):
    """Returns the Finder settings of the icon view, the background included."""
    return {
        "viewOptionsVersion": 1,
        "backgroundType": 2,
        "backgroundImageAlias": Alias.for_file(str(background)).to_bytes(),
        "backgroundColorRed": 1.0,
        "backgroundColorGreen": 1.0,
        "backgroundColorBlue": 1.0,
        "gridOffsetX": 0.0,
        "gridOffsetY": 0.0,
        "gridSpacing": 100.0,
        "arrangeBy": "none",
        "showIconPreview": False,
        "showItemInfo": False,
        "labelOnBottom": True,
        "textSize": float(TEXT_SIZE),
        "iconSize": float(ICON_SIZE),
        "scrollPositionX": 0.0,
        "scrollPositionY": 0.0,
    }


def write_ds_store(mount_point):
    """Writes the .DS_Store on the volume, then copies it next to this script."""
    background = mount_point / ".background" / BACKGROUND_NAME
    store_path = mount_point / ".DS_Store"
    with DSStore.open(str(store_path), "w+") as store:
        store["."]["bwsp"] = window_settings()
        store["."]["icvp"] = icon_settings(background)
        store["."]["vSrn"] = ("long", 1)
        store["R-Type.app"]["Iloc"] = APP_CENTER
        store["Applications"]["Iloc"] = APPLICATIONS_CENTER
    shutil.copyfile(store_path, FOLDER / DS_STORE_NAME)


def mount_scratch_volume(work):
    """Creates and mounts an empty volume with the name of the client image."""
    image = work / "layout.dmg"
    mount_point = work / "volume"
    mount_point.mkdir()
    subprocess.run(
        [
            "hdiutil", "create", "-quiet", "-size", "8m", "-fs", "HFS+",
            "-volname", VOLUME_NAME, str(image),
        ],
        check=True,
    )
    subprocess.run(
        [
            "hdiutil", "attach", "-quiet", "-nobrowse", "-noautoopen",
            "-mountpoint", str(mount_point), str(image),
        ],
        check=True,
    )
    return mount_point


def main():
    """Writes background.tiff and DS_Store in the folder of this script."""
    if sys.platform != "darwin":
        sys.exit("make_layout.py needs macOS: it mounts a disk image")
    with tempfile.TemporaryDirectory() as folder:
        work = Path(folder).resolve()
        write_background(work)
        mount_point = mount_scratch_volume(work)
        try:
            (mount_point / ".background").mkdir()
            shutil.copyfile(FOLDER / BACKGROUND_NAME, mount_point / ".background" / BACKGROUND_NAME)
            (mount_point / "R-Type.app").mkdir()
            os.symlink("/Applications", mount_point / "Applications")
            write_ds_store(mount_point)
        finally:
            subprocess.run(["hdiutil", "detach", "-quiet", str(mount_point)], check=True)


if __name__ == "__main__":
    main()
