--!strict
-- The level the resource ashlands brings: THE ASHLANDS, a map the players of
-- a waiting room vote for, in four stages, each in a biome of its own.
--
-- 1, Highway Zero (biome cendres): the burnt desert, the broken highway and
-- the checkpoint, under an open sky; Cindermites, Dunecrawlers and
-- Scrapdrones, then the Sandmaw (server/sandmaw.luau), planted in the floor of
-- the checkpoint.
-- 2, Shelter 9 (cendres_abri): its gate, dormitory and machine room, a
-- corridor whose blast doors decide where the ships fly; Rail turrets, Geiger
-- mines and Fixbots.
-- 3, the core (cendres_coeur): a short way in, then the Glowheart
-- (server/glowheart.luau) among the steam vents that purge with it.
-- 4, Silo Zero (cendres_silo): the floor of the core gives way into the nest
-- (a transition), then the Ash Mother (server/ash_mother.luau).
--
-- The mobs are in server/ashlands_mobs.luau, the decor that does something in
-- server/ashlands_props.luau. `--stage <n>` starts a game at a stage. The
-- positions are written here until the levels place their enemies in their
-- LDtk projects.

-- Past the right edge of the playfield, where an enemy enters, and where a
-- piece of decor appears for the scroll to bring it in.
local ENTRY_COLUMN = 1984
local DECOR_COLUMN = 2040

-- The scroll each stage ends at.
local END_OF_THE_HIGHWAY = 5280
local END_OF_THE_SHELTER = 10800
local END_OF_THE_CORE = 12960
local END_OF_THE_DESCENT = 15600

-- The rows the maps leave free: the ceiling and the floor of every corridor
-- of the Ashlands (the highway has no ceiling), which the Rail turrets and the
-- Dunecrawlers keep; and the row the taller Fixbots roll along.
local CEILING_ROW, FLOOR_ROW = 192, 848
local OPEN_SKY_ROW = 0
local ROLL_ROW = 840

-- Where the center of each piece of decor stands so that it rests on the
-- floor (row 896), hangs from the ceiling (row 192) or sits on the wall.
local STANDING_ON_THE_FLOOR = {
	toxic_barrel = 860,
	billboard = 836,
	rad_geyser = 700,
	searchlight = 700,
	steam_vent = 700,
	control_rods = 772,
}
local HANGING_FROM_THE_CEILING = {
	live_cable = 376,
	coolant_leak = 252,
	vent_grille = 236,
}
local ON_THE_WALL = {
	alarm_beacon = 260,
	meltdown_lamp = 300,
}

-- A blast door leaves a gap GAP units tall around its row; each half is 480
-- units tall.
local DOOR_GAP = 260
local DOOR_HALF = 480

-- Where each boss appears: the Sandmaw under the floor by the right edge,
-- where it climbs to and stays, the Glowheart past the right edge on the
-- middle row, the Ash Mother under the floor of the silo where she stands.
local SANDMAW_ENTRY = { x = 1660, y = 1160 }
local GLOWHEART_ENTRY = { x = 2200, y = 520 }
local ASH_MOTHER_ENTRY = { x = 1608, y = 1320 }

-- How long the floor of the core takes to give way into the silo.
local INTO_THE_SILO_MS = 4000

-- How often a stage looks at the sky, in milliseconds.
local SKY_CHECK_MS = 250

type Wave = { at: number, sheet: string, rows: { number } }
type Decor = { at: number, sheet: string, row: number? }

local function send(sheet: string, rows: { number })
	for _, row in rows do
		behaviours.spawn(sheet, { x = ENTRY_COLUMN, y = row })
	end
end

local function row_of(sheet: string): number
	return STANDING_ON_THE_FLOOR[sheet] or HANGING_FROM_THE_CEILING[sheet] or ON_THE_WALL[sheet] or FLOOR_ROW
end

-- A blast door is two halves around the row of its gap.
local function place(decor: Decor)
	if decor.sheet == "blast_door" then
		local gap = decor.row or FLOOR_ROW
		local away = DOOR_GAP / 2 + DOOR_HALF / 2
		behaviours.spawn("blast_door", { x = DECOR_COLUMN, y = gap - away })
		behaviours.spawn("blast_door_lower", { x = DECOR_COLUMN, y = gap + away })
		return
	end
	behaviours.spawn(decor.sheet, { x = DECOR_COLUMN, y = decor.row or row_of(decor.sheet) })
end

-- Plays the waves and lays the decor in the order of their scroll.
local function play(waves: { Wave }, decors: { Decor })
	local next_wave, next_decor = 1, 1
	while next_wave <= #waves or next_decor <= #decors do
		local wave, decor = waves[next_wave], decors[next_decor]
		if decor == nil or (wave ~= nil and wave.at <= decor.at) then
			level.wait_scroll(wave.at)
			send(wave.sheet, wave.rows)
			next_wave += 1
		else
			level.wait_scroll(decor.at)
			place(decor)
			next_decor += 1
		end
	end
end

local function wait_for_a_clear_sky()
	while level.enemies() > 0 do
		level.wait(SKY_CHECK_MS)
	end
end

-- A boss is a body like any other: the players are warned of it once it
-- appeared, at the end of the tick, so the stage waits before it looks at the
-- sky. The hazards on screen are removed once the boss is killed.
local function fight(boss: string, entry: { x: number, y: number })
	level.pause_scroll()
	behaviours.spawn(boss, entry)
	level.wait(SKY_CHECK_MS)
	wait_for_a_clear_sky()
	level.resume_scroll()
end

local SWARM_HIGH = { 280, 360, 440, 520, 600 }
local SWARM_LOW = { 480, 560, 640, 720, 800 }

local HIGHWAY: { Wave } = {
	{ at = 360, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 960, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 1320, sheet = "cindermite", rows = SWARM_LOW },
	{ at = 1680, sheet = "scrapdrone", rows = { 360, 700 } },
	{ at = 2160, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 2400, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 2760, sheet = "scrapdrone", rows = { 300, 540, 780 } },
	{ at = 3120, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 3480, sheet = "cindermite", rows = SWARM_LOW },
	{ at = 4200, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 4560, sheet = "scrapdrone", rows = { 420, 660 } },
	{ at = 4800, sheet = "cindermite", rows = SWARM_HIGH },
}

local HIGHWAY_DECOR: { Decor } = {
	{ at = 720, sheet = "rad_geyser" },
	{ at = 1200, sheet = "toxic_barrel" },
	{ at = 2040, sheet = "rad_geyser" },
	{ at = 2280, sheet = "billboard" },
	{ at = 3000, sheet = "toxic_barrel" },
	{ at = 3720, sheet = "searchlight" },
	{ at = 4080, sheet = "rad_geyser" },
	{ at = 4320, sheet = "toxic_barrel" },
}

local SHELTER: { Wave } = {
	{ at = 5760, sheet = "rail_turret", rows = { CEILING_ROW } },
	{ at = 6000, sheet = "fixbot", rows = { ROLL_ROW } },
	{ at = 6240, sheet = "geiger_mine", rows = { 360, 680 } },
	{ at = 7560, sheet = "rail_turret", rows = { CEILING_ROW } },
	{ at = 7680, sheet = "fixbot", rows = { ROLL_ROW } },
	{ at = 7920, sheet = "geiger_mine", rows = { 300, 520, 740 } },
	{ at = 8880, sheet = "rail_turret", rows = { CEILING_ROW } },
	{ at = 9240, sheet = "fixbot", rows = { ROLL_ROW } },
	{ at = 9600, sheet = "geiger_mine", rows = { 420, 640 } },
	{ at = 9960, sheet = "rail_turret", rows = { CEILING_ROW } },
	{ at = 10200, sheet = "cindermite", rows = SWARM_HIGH },
}

local SHELTER_DECOR: { Decor } = {
	{ at = 5520, sheet = "blast_door", row = 520 },
	{ at = 6480, sheet = "live_cable" },
	{ at = 6720, sheet = "alarm_beacon" },
	{ at = 7080, sheet = "vent_grille" },
	{ at = 7320, sheet = "blast_door", row = 360 },
	{ at = 8280, sheet = "live_cable" },
	{ at = 8400, sheet = "live_cable" },
	{ at = 8640, sheet = "blast_door", row = 700 },
	{ at = 8880, sheet = "vent_grille" },
}

local INTO_THE_CORE: { Wave } = {
	{ at = 11040, sheet = "geiger_mine", rows = { 360, 680 } },
	{ at = 11520, sheet = "scrapdrone", rows = { 300, 740 } },
}

-- The decor of the core: the way in, then the arena, laid so that it stands
-- where the scroll pauses for the fight: a piece laid at DECOR_COLUMN stands
-- at x once the level scrolled DECOR_COLUMN - x more.
local CORE_DECOR: { Decor } = {
	{ at = 10920, sheet = "control_rods" },
	{ at = END_OF_THE_CORE - 1740, sheet = "coolant_leak" },
	{ at = END_OF_THE_CORE - 1640, sheet = "steam_vent" },
	{ at = END_OF_THE_CORE - 1340, sheet = "meltdown_lamp" },
	{ at = END_OF_THE_CORE - 1140, sheet = "coolant_leak" },
	{ at = END_OF_THE_CORE - 1040, sheet = "steam_vent" },
}

local DESCENT: { Wave } = {
	{ at = 13200, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 13560, sheet = "brood_mite", rows = { 300, 420, 540 } },
	{ at = 13920, sheet = "cindermite", rows = SWARM_LOW },
	{ at = 14280, sheet = "brood_mite", rows = { 480, 600, 720 } },
	{ at = 14640, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 14880, sheet = "brood_mite", rows = { 300, 520, 740 } },
}

level.register("ashlands", {
	function()
		play(HIGHWAY, HIGHWAY_DECOR)
		level.wait_scroll(END_OF_THE_HIGHWAY)
		wait_for_a_clear_sky()
		fight("sandmaw", SANDMAW_ENTRY)
	end,
	function()
		level.biome("cendres_abri")
		play(SHELTER, SHELTER_DECOR)
		level.wait_scroll(END_OF_THE_SHELTER)
		wait_for_a_clear_sky()
	end,
	function()
		level.biome("cendres_coeur")
		play(INTO_THE_CORE, CORE_DECOR)
		level.wait_scroll(END_OF_THE_CORE)
		fight("glowheart", GLOWHEART_ENTRY)
	end,
	function()
		level.biome("cendres_silo")
		level.transition(INTO_THE_SILO_MS)
		play(DESCENT, {})
		level.wait_scroll(END_OF_THE_DESCENT)
		wait_for_a_clear_sky()
		fight("ash_mother", ASH_MOTHER_ENTRY)
	end,
}, {
	map = "THE ASHLANDS",
	preview = "the_ashlands",
	biome = "cendres",
	starts = {
		[2] = { scroll = END_OF_THE_HIGHWAY, biome = "cendres_abri", biome_since = 0 },
		[3] = { scroll = END_OF_THE_SHELTER, biome = "cendres_coeur", biome_since = 0 },
		[4] = { scroll = END_OF_THE_CORE, biome = "cendres_silo", biome_since = 0 },
	},
	bands = {
		cendres = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
		cendres_abri = { top = CEILING_ROW, bottom = FLOOR_ROW },
		cendres_coeur = { top = CEILING_ROW, bottom = FLOOR_ROW },
		cendres_silo = { top = CEILING_ROW, bottom = FLOOR_ROW },
	},
})
