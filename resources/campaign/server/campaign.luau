--!strict
-- The level campaign: five stages a team plays one after the other, each one
-- after the first starting at a checkpoint. A server plays it with
-- `set level campaign`.
--
-- The first two stages cross open space, with the stars alone; the second
-- ends on a mini-boss, the Bydo Tyrant with 40 % of its health. The third
-- stage enters the Pit through its cave, and the last
-- ends on the Bydo Tyrant at full health. In the Pit, the Bydos keep to the
-- rows the maps leave free, from 192 to 848 logical units. The positions are
-- written here until the level places its enemies in its LDtk project.

-- Past the right edge of the playfield, where a Bydo enters.
local ENTRY_COLUMN = 1984

-- The share of its health the Bydo Tyrant has as a mini-boss.
local MINI_BOSS_HEALTH_PERCENT = 40

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
		level.wait(250)
	end
end

local function fight(boss: string, options: { health_percent: number }?)
	level.pause_scroll()
	level.boss(boss, options)
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
	{ at = 5880, sheet = "drifter", rows = { 300, 540, 760 } },
	{ at = 6600, sheet = "weaver", rows = { 360, 720 } },
	{ at = 7320, sheet = "voltjelly", rows = { 300, 760 } },
	{ at = 8040, sheet = "drifter", rows = { 240, 420, 600, 780 } },
	{ at = 8760, sheet = "reaper", rows = { 540 } },
	{ at = 9480, sheet = "weaver", rows = { 300, 540, 760 } },
	{ at = 10200, sheet = "drillback", rows = { 540 } },
}

local DEPTHS: { Wave } = {
	{ at = 11160, sheet = "bydomancer", rows = { 360, 720 } },
	{ at = 11880, sheet = "drillback", rows = { 300, 760 } },
	{ at = 12600, sheet = "reaper", rows = { 300, 540, 760 } },
	{ at = 13320, sheet = "voltjelly", rows = { 240, 420, 600, 780 } },
	{ at = 14040, sheet = "bydomancer", rows = { 540 } },
	{ at = 14760, sheet = "weaver", rows = { 300, 540, 760 } },
	{ at = 15480, sheet = "drillback", rows = { 360, 720 } },
}

level.register("campaign", {
	function()
		play(OUT_OF_THE_BASE)
		level.wait_scroll(3000)
	end,
	function()
		play(BYDO_FIELD)
		level.wait_scroll(5400)
		wait_for_a_clear_sky()
		fight("bydo_tyrant", { health_percent = MINI_BOSS_HEALTH_PERCENT })
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
		fight("bydo_tyrant")
	end,
})
