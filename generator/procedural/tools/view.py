"""Cuts a generated map into its zones, doubled, so each one can be looked at closely."""
import sys

from PIL import Image

source, target = sys.argv[1], sys.argv[2]
image = Image.open(source)
for zone in range(image.width // 480):
    image.crop((zone * 480, 0, zone * 480 + 480, 272)).resize((960, 544), Image.NEAREST).save(f"{target}_{zone}.png")
