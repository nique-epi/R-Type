--!strict
-- The level the resource ashlands brings: THE ASHLANDS, a map the players of
-- a waiting room vote for, in six stages across four biomes.
--
-- 1, Highway Zero (biome cendres): the burnt desert, the broken highway and
-- the checkpoint, under an open sky; Cindermites, Dunecrawlers and
-- Scrapdrones.
-- 2, the checkpoint: the Sandmaw (server/sandmaw.luau), planted in its floor.
-- 3, Shelter 9 (cendres_abri): its gate, dormitory and machine room, a
-- corridor whose blast doors decide where the ships fly; Rail turrets, Geiger
-- mines and Fixbots. The core arrives as it ends, so it has fully come in by
-- the fight of the next stage.
-- 4, the core (cendres_coeur): a short way in, then the Glowheart
-- (server/glowheart.luau) among the steam vents that purge with it.
-- 5, Silo Zero (cendres_silo): the way down into the nest, arriving through
-- its entrance as the scroll goes on.
-- 6, the nest: the Ash Mother (server/ash_mother.luau).
--
-- Each biome is named at the end of the stage before the one it frames, after
-- its fight: a checkpoint is the start of a stage, so a team sent back to it
-- finds the biome already there; a game started at a stage with `--stage`
-- gets it from the option starts.
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
-- Dunecrawlers keep; and the row the taller Fixbots roll along. The surfaces
-- are where a corridor has its floor and its ceiling on average, used where
-- the biome gives no ground (level.ground and level.ceiling read nil).
local CEILING_ROW, FLOOR_ROW = 192, 848
local OPEN_SKY_ROW = 0
local ROLL_ROW = 840
local FLOOR_SURFACE, CEILING_SURFACE = 896, 192

-- How far the center of each piece of decor stands above the floor, or hangs
-- below the ceiling, so that it rests on it: the top of what hangs, the base
-- of the beacon and the frame of the lamp included, touches it. It is laid on
-- the ground of the biome under its column (level.ground, level.ceiling).
local ABOVE_THE_FLOOR = {
	toxic_barrel = 36,
	billboard = 60,
	rad_geyser = 196,
	searchlight = 196,
	steam_vent = 196,
	control_rods = 124,
}
local BELOW_THE_CEILING = {
	live_cable = 184,
	coolant_leak = 60,
	vent_grille = 44,
	alarm_beacon = 24,
	meltdown_lamp = 40,
}

-- The same for the mobs that walk on the floor or slide under the ceiling,
-- where they enter; their behaviours keep them there with the same gaps
-- (server/ashlands_mobs.luau).
local WALKING_ON_THE_FLOOR = { dunecrawler = 48, fixbot = 56 }
local SLIDING_UNDER_THE_CEILING = { rail_turret = 0 }

-- A blast door leaves a gap DOOR_GAP units tall around its row, five ships;
-- each half is 480 units tall.
local DOOR_GAP = 340
local DOOR_HALF = 480

-- Where each boss appears: the Sandmaw under the floor by the right edge,
-- where it climbs to and stays, the Glowheart past the right edge on the
-- middle row, the Ash Mother under the floor of the silo where she stands.
local SANDMAW_ENTRY = { x = 1660, y = 1160 }
local GLOWHEART_ENTRY = { x = 2200, y = 520 }
local ASH_MOTHER_ENTRY = { x = 1608, y = 1320 }

-- How often a stage looks for its boss, in milliseconds.
local SKY_CHECK_MS = 250

type Wave = { at: number, sheet: string, rows: { number } }
type Decor = { at: number, sheet: string, row: number? }

-- The row of the center of `sheet` resting on the ground at `column`, if it
-- rests on the floor (`above`) or hangs from the ceiling (`below`); nil for a
-- body that does neither.
local function grounded_row(sheet: string, column: number, above, below): number?
	if above[sheet] ~= nil then
		return (level.ground(column) or FLOOR_SURFACE) - above[sheet]
	end
	if below[sheet] ~= nil then
		return (level.ceiling(column) or CEILING_SURFACE) + below[sheet]
	end
	return nil
end

local function send(sheet: string, rows: { number })
	local grounded = grounded_row(sheet, ENTRY_COLUMN, WALKING_ON_THE_FLOOR, SLIDING_UNDER_THE_CEILING)
	for _, row in rows do
		behaviours.spawn(sheet, { x = ENTRY_COLUMN, y = grounded or row })
	end
end

local function row_of(sheet: string, column: number): number
	return grounded_row(sheet, column, ABOVE_THE_FLOOR, BELOW_THE_CEILING) or FLOOR_ROW
end

-- Lays a piece of decor where the scroll has brought it: at DECOR_COLUMN on
-- its scroll, further left when the stage reaches it late, so two pieces
-- never appear on top of each other. A
-- blast door is two halves around the row of its gap.
local function place(decor: Decor)
	local column = DECOR_COLUMN - math.max(0, level.scroll() - decor.at)
	if decor.sheet == "blast_door" then
		local gap = decor.row or FLOOR_ROW
		local away = DOOR_GAP / 2 + DOOR_HALF / 2
		behaviours.spawn("blast_door", { x = column, y = gap - away })
		behaviours.spawn("blast_door_lower", { x = column, y = gap + away })
		return
	end
	behaviours.spawn(decor.sheet, { x = column, y = decor.row or row_of(decor.sheet, column) })
end

-- Plays the waves and lays the decor in the order of their scroll, each
-- `shift` units later than written.
local function play(waves: { Wave }, decors: { Decor }, shift: number)
	local next_wave, next_decor = 1, 1
	while next_wave <= #waves or next_decor <= #decors do
		local wave, decor = waves[next_wave], decors[next_decor]
		if decor == nil or (wave ~= nil and wave.at <= decor.at) then
			level.wait_scroll(wave.at + shift)
			send(wave.sheet, wave.rows)
			next_wave += 1
		else
			level.wait_scroll(decor.at + shift)
			place({ at = decor.at + shift, sheet = decor.sheet, row = decor.row })
			next_decor += 1
		end
	end
end

-- How much later than `planned` the stage that starts now begins. The waves
-- and the decor of a stage are written for a stage that starts at the scroll
-- its biome begins, `planned`; a stage before it that ran late names the
-- biome later, and the stage then plays everything that much later,
-- so no piece of its decor is laid left of where its biome comes in, over the
-- biome that leaves. A stage restarted from its checkpoint starts where it
-- started the first time.
local function lateness(planned: number): number
	return math.max(0, level.scroll() - planned)
end

-- Waits until no boss is left: the mobs still on screen never hold the
-- level, they fly on and leave as the scroll goes on.
local function wait_for_the_boss()
	while level.bosses() > 0 do
		level.wait(SKY_CHECK_MS)
	end
end

-- A boss is a body like any other: the players are warned of it once it
-- appeared, at the end of the tick, so the stage waits before it looks for it.
-- The scroll moves on once the boss is killed, whatever mobs are left. The hazards on screen are removed once the boss is killed.
local function fight(boss: string, entry: { x: number, y: number })
	level.pause_scroll()
	behaviours.spawn(boss, entry)
	level.wait(SKY_CHECK_MS)
	wait_for_the_boss()
	level.resume_scroll()
end

local SWARM_HIGH = { 280, 360, 440, 520, 600 }
local SWARM_LOW = { 480, 560, 640, 720, 800 }

local HIGHWAY: { Wave } = {
	{ at = 360, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 600, sheet = "crystal_carrier", rows = { 540 } },
	{ at = 960, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 1320, sheet = "cindermite", rows = SWARM_LOW },
	{ at = 1680, sheet = "scrapdrone", rows = { 360, 700 } },
	{ at = 2160, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 2400, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 2580, sheet = "crystal_carrier", rows = { 540 } },
	{ at = 2760, sheet = "scrapdrone", rows = { 300, 540, 780 } },
	{ at = 3120, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 3480, sheet = "cindermite", rows = SWARM_LOW },
	{ at = 4200, sheet = "dunecrawler", rows = { FLOOR_ROW } },
	{ at = 4560, sheet = "scrapdrone", rows = { 420, 660 } },
	{ at = 4800, sheet = "cindermite", rows = SWARM_HIGH },
	{ at = 4920, sheet = "crystal_carrier", rows = { 540 } },
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
	{ at = 6480, sheet = "geiger_mine", rows = { 520 } },
	{ at = 7200, sheet = "fixbot", rows = { ROLL_ROW } },
	{ at = 7920, sheet = "rail_turret", rows = { CEILING_ROW } },
	{ at = 8640, sheet = "geiger_mine", rows = { 300, 740 } },
	{ at = 9360, sheet = "fixbot", rows = { ROLL_ROW } },
	{ at = 10080, sheet = "cindermite", rows = SWARM_HIGH },
}

-- The decor of the shelter, a piece every 720 units of scroll at least, so
-- one never stands on another.
local SHELTER_DECOR: { Decor } = {
	{ at = 5520, sheet = "blast_door", row = 520 },
	{ at = 6120, sheet = "live_cable" },
	{ at = 6840, sheet = "alarm_beacon" },
	{ at = 7560, sheet = "vent_grille" },
	{ at = 8280, sheet = "blast_door", row = 360 },
	{ at = 9000, sheet = "live_cable" },
	{ at = 9720, sheet = "blast_door", row = 700 },
}

local INTO_THE_CORE: { Wave } = {
	{ at = 11280, sheet = "geiger_mine", rows = { 520 } },
	{ at = 12600, sheet = "crystal_carrier", rows = { 540 } },
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
	{ at = 15240, sheet = "crystal_carrier", rows = { 540 } },
}

level.register("ashlands", {
	function()
		play(HIGHWAY, HIGHWAY_DECOR, 0)
		level.wait_scroll(END_OF_THE_HIGHWAY)
	end,
	function()
		fight("sandmaw", SANDMAW_ENTRY)
		level.biome("cendres_abri")
	end,
	function()
		local shift = lateness(END_OF_THE_HIGHWAY)
		play(SHELTER, SHELTER_DECOR, shift)
		level.wait_scroll(END_OF_THE_SHELTER + shift)
		level.biome("cendres_coeur")
	end,
	function()
		local shift = lateness(END_OF_THE_SHELTER)
		play(INTO_THE_CORE, CORE_DECOR, shift)
		level.wait_scroll(END_OF_THE_CORE + shift)
		fight("glowheart", GLOWHEART_ENTRY)
		level.biome("cendres_silo")
	end,
	function()
		local shift = lateness(END_OF_THE_CORE)
		play(DESCENT, {}, shift)
		level.wait_scroll(END_OF_THE_DESCENT + shift)
	end,
	function()
		fight("ash_mother", ASH_MOTHER_ENTRY)
	end,
}, {
	map = "THE ASHLANDS",
	reward = 500,
	preview = "the_ashlands",
	biome = "cendres",
	starts = {
		[2] = { scroll = END_OF_THE_HIGHWAY, biome = "cendres", biome_since = 0 },
		[3] = { scroll = END_OF_THE_HIGHWAY, biome = "cendres_abri", biome_since = 0 },
		[4] = { scroll = END_OF_THE_SHELTER, biome = "cendres_coeur", biome_since = 0 },
		[5] = { scroll = END_OF_THE_CORE, biome = "cendres_silo", biome_since = 0 },
		[6] = { scroll = END_OF_THE_DESCENT, biome = "cendres_silo", biome_since = 0 },
	},
	bands = {
		cendres = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
		cendres_abri = { top = CEILING_ROW, bottom = FLOOR_ROW },
		cendres_coeur = { top = CEILING_ROW, bottom = FLOOR_ROW },
		cendres_silo = { top = CEILING_ROW, bottom = FLOOR_ROW },
	},
})
