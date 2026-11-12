"""Sprites of the demonstration biome drawn in palette letters; the import tool renders them to pictures and imports them like any picture."""


def centered(rows):
    """Rows of uneven length padded with empty pixels to one width."""
    width = max(len(row) for row in rows)
    return [row.center(width, ".") for row in rows]


LINK = [".44.", "3..2", "3..2", ".22."]

DRAWN = {
    "forge.large.furnace.glow.hero": centered([
        "t4431", "43321", "43321", "43321",
        "t444444431", "4333333321",
        "t44444444444431", "433333333333321", "432222222222210",
        "4322dKKKKKd2210", "432dKeeeeeKd210", "432KeeeeeeeK210", "432KeeooooeK210", "432dKooooKKd210", "4322ddddddd2210",
        "432222222222210",
        "t4444444444444431", "43333333333333321", "43222222222222210",
    ]),
    "forge.large.anvil": centered([
        "t4444444444431", "43333333333210", "432222222210", "4322210", "4322210", "t44322110", "433332222110", "4322222221100",
    ]),
    "forge.medium.brazier.glow": centered([
        "d", "dKd.d", "dKeKdKd", "dKeeKKed",
        "t444444431", "43333321", "432221", "3221", "3221", "432210", "t4322110",
    ]),
    "forge.small.ingots": centered(["t441", "t4331t441", "t43321t3321"]),
    "chains.large.chain_long.hang": centered(LINK * 8 + [".44.", "t441", "dKKd", ".dd."]),
    "chains.medium.chain_hook.hang": centered(LINK * 4 + ["..44", "...3", "3..3", ".322"]),
    "chains.small.chain_short.hang": centered(LINK * 3),
}
