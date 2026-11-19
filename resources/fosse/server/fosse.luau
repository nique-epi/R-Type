--!strict
-- The two levels the resource fosse brings, each a map the players of a
-- waiting room vote for (the option map of level.register), each in one
-- biome. A server that names no level with `set level` offers both to the
-- vote; one that names a level plays it, and starts it at a stage with
-- `--stage <n>`.
--
-- space, OPEN SPACE: two stages that cross open space, with the stars alone,
-- the second ending on the Bydo Tyrant.
--
-- fosse, THE PIT: three stages in the Pit, already inside it from the start
-- (the option biome), where only the mobs of the Pit come (server/fosse_mobs.luau and the
-- Gloomwyrm of server/gloomwyrm.luau); the last one fights its boss, the
-- Necromancer (server/necromancer.luau). `--stage 2` starts in its depths,
-- `--stage 3` on the Necromancer. In the Pit, the enemies keep to the rows the
-- maps leave free, from 192 to 848 logical units, which no ship flies past
-- (the band of the biome): the Stalagmaw crawls along the top one, the
-- Sporecap walks along the bottom one. The positions are written here until
-- the levels place their enemies in their LDtk projects.

-- Past the right edge of the playfield, where an enemy enters.
local ENTRY_COLUMN = 1984

-- The scroll the open space ends at, where the Bydo Tyrant is fought: the
-- scroll is paused through the fight.
local END_OF_THE_OPEN_SPACE = 5400

-- The scroll the entry into the Pit and its depths end at, where the next
-- stage begins.
local END_OF_THE_PIT_ENTRY = 5280
local END_OF_THE_DEPTHS = 10800

-- Where the center of a boss appears: half its 512-unit frame past the right
-- edge, on the middle row of the area of play.
local BOSS_ENTRY = { x = 2176, y = 492 }

-- Where the center of the Necromancer appears: below the bottom edge, past
-- half its 440-unit frame, on the column it then rises along, its right side
-- by the right edge.
local NECROMANCER_ENTRY = { x = 1600, y = 1310 }

-- The top and the bottom rows the maps of the Pit leave free: its ceiling and
-- its floor.
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
	{ at = 480, sheet = "glowmoth", rows = { 300, 380, 460, 540, 620 } },
	{ at = 1200, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 1200, sheet = "glowmoth", rows = { 300, 380, 460, 540, 620 } },
	{ at = 1920, sheet = "lanternfin", rows = { 360, 720 } },
	{ at = 2640, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 2640, sheet = "lanternfin", rows = { 480, 720 } },
	{ at = 3360, sheet = "ruin_warden", rows = { 540 } },
	{ at = 3720, sheet = "gloomwyrm", rows = { 260 } },
	{ at = 4080, sheet = "glowmoth", rows = { 520, 600, 680, 760, 840 } },
	{ at = 4800, sheet = "ruin_warden", rows = { 360, 720 } },
}

local DEPTHS: { Wave } = {
	{ at = 5760, sheet = "ruin_warden", rows = { 300, 760 } },
	{ at = 6480, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 6480, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 7200, sheet = "lanternfin", rows = { 300, 540, 760 } },
	{ at = 7920, sheet = "glowmoth", rows = { 240, 320, 400, 480, 560 } },
	{ at = 8280, sheet = "gloomwyrm", rows = { 690 } },
	{ at = 8640, sheet = "ruin_warden", rows = { 540 } },
	{ at = 9360, sheet = "stalagmaw", rows = { CEILING_ROW } },
	{ at = 9360, sheet = "sporecap", rows = { FLOOR_ROW } },
	{ at = 10080, sheet = "lanternfin", rows = { 360, 720 } },
}

level.register("space", {
	function()
		play(OUT_OF_THE_BASE)
		level.wait_scroll(3000)
	end,
	function()
		play(BYDO_FIELD)
		level.wait_scroll(END_OF_THE_OPEN_SPACE)
		wait_for_a_clear_sky()
		fight("bydo_tyrant", BOSS_ENTRY)
	end,
}, { map = "OPEN SPACE" })

level.register("fosse", {
	function()
		play(INTO_THE_PIT)
		level.wait_scroll(END_OF_THE_PIT_ENTRY)
	end,
	function()
		play(DEPTHS)
		level.wait_scroll(END_OF_THE_DEPTHS)
		wait_for_a_clear_sky()
	end,
	function()
		fight("necromancer", NECROMANCER_ENTRY)
	end,
}, {
	map = "THE PIT",
	biome = "fosse",
	starts = {
		[2] = { scroll = END_OF_THE_PIT_ENTRY, biome = "fosse", biome_since = 0 },
		[3] = { scroll = END_OF_THE_DEPTHS, biome = "fosse", biome_since = 0 },
	},
	bands = {
		fosse = { top = CEILING_ROW, bottom = FLOOR_ROW },
	},
})
