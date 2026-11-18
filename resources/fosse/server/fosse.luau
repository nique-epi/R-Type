--!strict
-- The level fosse, named after the biome it leads into: five stages a team
-- plays one after the other, each one after the first starting at a
-- checkpoint. A server plays it with `set level fosse`.
--
-- The first two stages cross open space, with the stars alone; the second
-- ends on a mini-boss, the Bydo Tyrant with 40 % of its health. The third
-- stage enters the Pit through its cave, where the mobs of the Pit join the
-- Bydos (server/fosse_mobs.luau), and the last ends on the boss of the Pit,
-- the Necromancer (server/necromancer.luau). In the Pit, the enemies keep to
-- the rows the maps leave free, from 192 to 848 logical units: the Stalagmaw
-- crawls along the top one, the Sporecap walks along the bottom one. The
-- positions are written here until the level places its enemies in its LDtk
-- project.

-- Past the right edge of the playfield, where a Bydo enters.
local ENTRY_COLUMN = 1984

-- The share of its health the Bydo Tyrant has as a mini-boss.
local MINI_BOSS_HEALTH_PERCENT = 40

-- Where the center of a boss appears: half its 512-unit frame past the right
-- edge, on the middle row of the area of play.
local BOSS_ENTRY = { x = 2176, y = 492 }

-- Where the center of the Necromancer appears: half its 1132-unit frame past
-- the right edge, its smoke on the floor of the Pit.
local NECROMANCER_ENTRY = { x = 2486, y = 542 }

-- The top and the bottom rows the maps of the Pit leave free.
local CEILING_ROW, FLOOR_ROW = 192, 848

-- How often a stage looks at the sky, in milliseconds.
local SKY_CHECK_MS = 250

type Wave = { at: number, sheet: string, rows: { number } }

local function send(sheet: string, rows: { number })
	for _, row in rows do
		behaviours.spawn(sheet, { x = ENTRY_COLUMN, y = row })
	end
end

local function play(waves: { Wave })
	for _, wave in waves do
		level.wait_scroll(wave.at)
		send(wave.sheet, wave.rows)
	end
end

local function wait_for_a_clear_sky()
	while level.enemies() > 0 do
		level.wait(SKY_CHECK_MS)
	end
end

-- A boss is a body like any other: the players are warned of it once it
-- appeared, at the end of the tick, so the stage waits before it looks at the
-- sky.
local function fight(boss: string, entry: { x: number, y: number }, options: { health_percent: number }?)
	level.pause_scroll()
	behaviours.spawn(boss, entry, options)
	level.wait(SKY_CHECK_MS)
	wait_for_a_clear_sky()
	level.resume_scroll()
end

local OUT_OF_THE_BASE: { Wave } = {
	{ at = 240, sheet = "drifter", rows = { 240, 540, 840 } },
	{ at = 720, sheet = "weaver", rows = { 360, 720 } },
	{ at = 1200, sheet = "drifter", rows = { 180, 420, 660, 900 } },
	{ at = 1680, sheet = "voltjelly", rows = { 300, 780 } },
	{ at = 2160, sheet = "weaver", rows = { 240, 540, 840 } },
	{ at = 2640, sheet = "drifter", rows = { 420, 660 } },
}

local BYDO_FIELD: { Wave } = {
	{ at = 3120, sheet = "drillback", rows = { 540 } },
	{ at = 3480, sheet = "reaper", rows = { 300, 780 } },
	{ at = 3960, sheet = "voltjelly", rows = { 180, 420, 660, 900 } },
	{ at = 4440, sheet = "bydomancer", rows = { 540 } },
	{ at = 4920, sheet = "weaver", rows = { 300, 780 } },
}

local INTO_THE_PIT: { Wave } = {
	{ at = 5880, sheet = "glowmoth", rows = { 300, 380, 460, 540, 620 } },
	{ at = 6600, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 6600, sheet = "drifter", rows = { 300, 540 } },
	{ at = 7320, sheet = "lanternfin", rows = { 360, 720 } },
	{ at = 8040, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 8040, sheet = "weaver", rows = { 480, 720 } },
	{ at = 8760, sheet = "ruin_warden", rows = { 540 } },
	{ at = 9480, sheet = "glowmoth", rows = { 520, 600, 680, 760, 840 } },
	{ at = 10200, sheet = "drillback", rows = { 540 } },
}

local DEPTHS: { Wave } = {
	{ at = 11160, sheet = "ruin_warden", rows = { 300, 760 } },
	{ at = 11880, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 11880, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 12600, sheet = "lanternfin", rows = { 300, 540, 760 } },
	{ at = 13320, sheet = "glowmoth", rows = { 240, 320, 400, 480, 560 } },
	{ at = 14040, sheet = "bydomancer", rows = { 540 } },
	{ at = 14760, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 14760, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 15480, sheet = "lanternfin", rows = { 360, 720 } },
}

level.register("fosse", {
	function()
		play(OUT_OF_THE_BASE)
		level.wait_scroll(3000)
	end,
	function()
		play(BYDO_FIELD)
		level.wait_scroll(5400)
		wait_for_a_clear_sky()
		fight("bydo_tyrant", BOSS_ENTRY, { health_percent = MINI_BOSS_HEALTH_PERCENT })
	end,
	function()
		level.biome("fosse")
		play(INTO_THE_PIT)
		level.wait_scroll(10680)
	end,
	function()
		play(DEPTHS)
		level.wait_scroll(16200)
		wait_for_a_clear_sky()
	end,
	function()
		fight("necromancer", NECROMANCER_ENTRY)
	end,
})
