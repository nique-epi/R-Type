--!strict
-- The level the resource deseret brings: DESERET, a map the players of a
-- waiting room vote for, in six stages across Utah in 1869, under an open sky
-- but in the mine.
--
-- 1, the Salt Flats (biome deseret): the white salt pan, then a homestead;
-- Carrions, swarms of crickets, Shrouds and Tumblecages, and powder kegs
-- whose blasts light each other.
-- 2, the Hoodoo Field (deseret_hoodoos): Aeronauts, dust devils and
-- Surveyors behind the spires; crates of dynamite that throw rocks.
-- 3, the Line (deseret_line): Carrions and Shrouds along the abandoned
-- railroad, kegs along the track.
-- 4, the Iron Pilgrim (server/iron_pilgrim.luau), on the track.
-- 5, Sheol Mine (deseret_mine): a timbered gallery under a ceiling;
-- Lanterns, swarms of crickets and Surveyors; mine carts that roll, and crates
-- that knock props off the ceiling.
-- 6, Widow's Mesa (deseret_mesa): a short way up the mesa, a few Carrions,
-- then the Old Widow (server/old_widow.luau).
--
-- Each biome is named at the end of the stage before the one it frames, after
-- its fight: a checkpoint is the start of a stage, so a team sent back to it
-- finds the biome already there; a game started at a stage with `--stage`
-- gets it from the option starts. The fights are paused at least the width of
-- the playfield after their biome was named, so it has fully come in.
--
-- The mobs are in server/deseret_mobs.luau, the decor that does something in
-- server/deseret_props.luau. `--stage <n>` starts a game at a stage.

-- Past the right edge of the playfield, where an enemy enters, and where a
-- piece of decor appears for the scroll to bring it in.
local ENTRY_COLUMN = 1984
local DECOR_COLUMN = 2040

-- The scroll each stage ends at: each map is 5760 units wide, and the next
-- biome is named 480 units before it ends.
local END_OF_THE_FLATS = 5280
local END_OF_THE_HOODOOS = 10560
local END_OF_THE_LINE = 15840
local END_OF_THE_MINE = 21120
local FOOT_OF_THE_MESA = 23280

-- The rows the maps leave free: the floor of every map, the ceiling of the
-- mine (the other maps have none); and the surfaces where the biome gives no
-- ground (level.ground and level.ceiling read nil).
local OPEN_SKY_ROW, CEILING_ROW, FLOOR_ROW = 0, 192, 848
local FLOOR_SURFACE, CEILING_SURFACE = 904, 160

-- How far the center of each piece of decor stands above the floor so that
-- its base rests on it, and how far the center of each mob that walks stands
-- above it (server/deseret_mobs.luau keeps them there).
local ABOVE_THE_FLOOR = {
	powder_keg = 34,
	dynamite_crate = 36,
	mine_cart = 36,
	mine_cart_keg = 60,
}
local WALKING_ON_THE_FLOOR = { tumblecage = 40, surveyor = 60 }

-- How far below the ceiling a Lantern floats (server/deseret_mobs.luau keeps
-- it there).
local UNDER_THE_CEILING = { lantern_wisp = 140 }

-- Where each boss appears: the Iron Pilgrim past the right edge, on the
-- track; the Old Widow below the floor of the mesa, where she rises.
local IRON_PILGRIM_ENTRY = { x = 2200, y = 860 }
local OLD_WIDOW_ENTRY = { x = 1560, y = 1320 }

-- How often a stage looks at the sky, in milliseconds.
local SKY_CHECK_MS = 250

type Wave = { at: number, sheet: string, rows: { number } }
type Decor = { at: number, sheet: string }

-- The row of the center of `sheet` at `column`: resting on the floor or
-- hanging under the ceiling the biome draws there; nil for a body that does
-- neither.
local function grounded_row(sheet: string, column: number): number?
	if ABOVE_THE_FLOOR[sheet] ~= nil then
		return (level.ground(column) or FLOOR_SURFACE) - ABOVE_THE_FLOOR[sheet]
	end
	if WALKING_ON_THE_FLOOR[sheet] ~= nil then
		return (level.ground(column) or FLOOR_SURFACE) - WALKING_ON_THE_FLOOR[sheet]
	end
	if UNDER_THE_CEILING[sheet] ~= nil then
		return (level.ceiling(column) or CEILING_SURFACE) + UNDER_THE_CEILING[sheet]
	end
	return nil
end

local function send(sheet: string, rows: { number })
	local grounded = grounded_row(sheet, ENTRY_COLUMN)
	for _, row in rows do
		behaviours.spawn(sheet, { x = ENTRY_COLUMN, y = grounded or row })
	end
end

-- Lays a piece of decor where the scroll has brought it: at DECOR_COLUMN on
-- its scroll, further left when the stage reaches it late, on the floor under
-- that column.
local function place(decor: Decor)
	local column = DECOR_COLUMN - math.max(0, level.scroll() - decor.at)
	behaviours.spawn(decor.sheet, { x = column, y = grounded_row(decor.sheet, column) or FLOOR_ROW })
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
			place({ at = decor.at + shift, sheet = decor.sheet })
			next_decor += 1
		end
	end
end

-- How much later than `planned` the stage that starts now begins: a stage
-- before it that waited for a clear sky names the biome later, and the stage
-- then plays everything that much later.
local function lateness(planned: number): number
	return math.max(0, level.scroll() - planned)
end

local function wait_for_a_clear_sky()
	while level.enemies() > 0 do
		level.wait(SKY_CHECK_MS)
	end
end

-- A boss is a body like any other: the players are warned of it once it
-- appeared, at the end of the tick, so the stage waits before it looks at the
-- sky.
local function fight(boss: string, entry: { x: number, y: number })
	level.pause_scroll()
	behaviours.spawn(boss, entry)
	level.wait(SKY_CHECK_MS)
	wait_for_a_clear_sky()
	level.resume_scroll()
end

local SWARM_HIGH = { 240, 300, 360, 420, 480, 540 }
local SWARM_LOW = { 480, 540, 600, 660, 720, 780 }
local SWARM_WIDE = { 260, 360, 460, 560, 660 }

local FLATS: { Wave } = {
	{ at = 360, sheet = "carrion", rows = { 300, 520 } },
	{ at = 600, sheet = "vultureling", rows = { 220, 340, 460 } },
	{ at = 840, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 1080, sheet = "carrion", rows = { 420 } },
	{ at = 1320, sheet = "tumblecage", rows = { FLOOR_ROW } },
	{ at = 1500, sheet = "vultureling", rows = { 260, 400 } },
	{ at = 1680, sheet = "shroud", rows = { 420 } },
	{ at = 1920, sheet = "vultureling", rows = { 240, 360, 480 } },
	{ at = 2160, sheet = "carrion", rows = { 260, 460, 660 } },
	{ at = 2400, sheet = "vultureling", rows = { 280, 420 } },
	{ at = 2640, sheet = "cricket", rows = SWARM_LOW },
	{ at = 2880, sheet = "vultureling", rows = { 220, 340, 460 } },
	{ at = 3000, sheet = "tumblecage", rows = { FLOOR_ROW } },
	{ at = 3120, sheet = "carrion", rows = { 380, 620 } },
	{ at = 3360, sheet = "shroud", rows = { 300, 700 } },
	{ at = 3600, sheet = "vultureling", rows = { 260, 380, 500 } },
	{ at = 3840, sheet = "carrion", rows = { 360, 600 } },
	{ at = 4020, sheet = "vultureling", rows = { 240, 400 } },
	{ at = 4200, sheet = "tumblecage", rows = { FLOOR_ROW } },
	{ at = 4380, sheet = "carrion", rows = { 300, 620 } },
	{ at = 4560, sheet = "cricket", rows = SWARM_WIDE },
	{ at = 4800, sheet = "vultureling", rows = { 240, 360, 480 } },
	{ at = 5040, sheet = "carrion", rows = { 400, 620 } },
}

-- Kegs in pairs and threes, close enough that one blast lights the next.
local FLATS_DECOR: { Decor } = {
	{ at = 1080, sheet = "powder_keg" },
	{ at = 1200, sheet = "powder_keg" },
	{ at = 2400, sheet = "powder_keg" },
	{ at = 2520, sheet = "powder_keg" },
	{ at = 2640, sheet = "powder_keg" },
	{ at = 3720, sheet = "powder_keg" },
	{ at = 3840, sheet = "powder_keg" },
}

local HOODOOS: { Wave } = {
	{ at = 5460, sheet = "vultureling", rows = { 260, 400 } },
	{ at = 5640, sheet = "aeronaut", rows = { 220 } },
	{ at = 5880, sheet = "vultureling", rows = { 300, 420, 540 } },
	{ at = 6120, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 6360, sheet = "carrion", rows = { 360, 580 } },
	{ at = 6600, sheet = "dust_devil", rows = { 560 } },
	{ at = 6840, sheet = "vultureling", rows = { 240, 380, 520 } },
	{ at = 7080, sheet = "aeronaut", rows = { 200, 300 } },
	{ at = 7320, sheet = "cricket", rows = SWARM_WIDE },
	{ at = 7560, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 7800, sheet = "carrion", rows = { 300, 560 } },
	{ at = 8040, sheet = "dust_devil", rows = { 440 } },
	{ at = 8220, sheet = "vultureling", rows = { 260, 400 } },
	{ at = 8400, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 8640, sheet = "vultureling", rows = { 280, 420, 560 } },
	{ at = 8880, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 9060, sheet = "carrion", rows = { 420, 640 } },
	{ at = 9240, sheet = "aeronaut", rows = { 240 } },
	{ at = 9480, sheet = "vultureling", rows = { 280, 420, 560 } },
	{ at = 9720, sheet = "dust_devil", rows = { 620 } },
	{ at = 9960, sheet = "cricket", rows = SWARM_LOW },
	{ at = 10200, sheet = "carrion", rows = { 320, 560 } },
	{ at = 10380, sheet = "vultureling", rows = { 300, 460 } },
}

local HOODOOS_DECOR: { Decor } = {
	{ at = 6360, sheet = "dynamite_crate" },
	{ at = 7800, sheet = "dynamite_crate" },
	{ at = 9000, sheet = "dynamite_crate" },
}

local THE_LINE: { Wave } = {
	{ at = 10740, sheet = "vultureling", rows = { 260, 400, 540 } },
	{ at = 10920, sheet = "carrion", rows = { 300, 500 } },
	{ at = 11160, sheet = "vultureling", rows = { 240, 380 } },
	{ at = 11400, sheet = "shroud", rows = { 360 } },
	{ at = 11640, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 11880, sheet = "carrion", rows = { 240, 440, 640 } },
	{ at = 12120, sheet = "vultureling", rows = { 300, 460, 620 } },
	{ at = 12360, sheet = "shroud", rows = { 600 } },
	{ at = 12600, sheet = "cricket", rows = SWARM_LOW },
	{ at = 12840, sheet = "carrion", rows = { 320, 560 } },
	{ at = 13080, sheet = "vultureling", rows = { 260, 420 } },
	{ at = 13320, sheet = "shroud", rows = { 300, 680 } },
	{ at = 13560, sheet = "cricket", rows = SWARM_WIDE },
	{ at = 13800, sheet = "carrion", rows = { 260, 460, 660 } },
	{ at = 14040, sheet = "vultureling", rows = { 280, 440, 600 } },
	{ at = 14280, sheet = "shroud", rows = { 480 } },
	{ at = 14520, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 14760, sheet = "carrion", rows = { 300, 500, 700 } },
	{ at = 15000, sheet = "vultureling", rows = { 300, 480 } },
	{ at = 15240, sheet = "shroud", rows = { 360, 620 } },
	{ at = 15480, sheet = "carrion", rows = { 360, 580 } },
	{ at = 15660, sheet = "vultureling", rows = { 260, 420 } },
}

-- Kegs along the track.
local THE_LINE_DECOR: { Decor } = {
	{ at = 11160, sheet = "powder_keg" },
	{ at = 11280, sheet = "powder_keg" },
	{ at = 12600, sheet = "powder_keg" },
	{ at = 12720, sheet = "powder_keg" },
	{ at = 14040, sheet = "powder_keg" },
	{ at = 14160, sheet = "powder_keg" },
}

local MINE: { Wave } = {
	{ at = 16020, sheet = "dust_bat", rows = { 250, 300, 350, 400 } },
	{ at = 16200, sheet = "lantern_wisp", rows = { CEILING_ROW } },
	{ at = 16440, sheet = "dust_bat", rows = { 260, 320, 380 } },
	{ at = 16680, sheet = "cricket", rows = SWARM_LOW },
	{ at = 16920, sheet = "dust_bat", rows = { 250, 300, 350, 400 } },
	{ at = 17160, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 17400, sheet = "carrion", rows = { 420, 620 } },
	{ at = 17640, sheet = "lantern_wisp", rows = { CEILING_ROW } },
	{ at = 17880, sheet = "dust_bat", rows = { 250, 310, 370, 430 } },
	{ at = 18120, sheet = "cricket", rows = SWARM_WIDE },
	{ at = 18360, sheet = "dust_bat", rows = { 260, 320, 380 } },
	{ at = 18600, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 18840, sheet = "carrion", rows = { 400, 600 } },
	{ at = 19080, sheet = "lantern_wisp", rows = { CEILING_ROW } },
	{ at = 19320, sheet = "dust_bat", rows = { 250, 300, 350, 400 } },
	{ at = 19560, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 19800, sheet = "dust_bat", rows = { 260, 320, 380 } },
	{ at = 20040, sheet = "surveyor", rows = { FLOOR_ROW } },
	{ at = 20280, sheet = "dust_bat", rows = { 250, 310, 370, 430 } },
	{ at = 20520, sheet = "cricket", rows = SWARM_LOW },
	{ at = 20760, sheet = "dust_bat", rows = { 260, 320, 380 } },
	{ at = 20940, sheet = "lantern_wisp", rows = { CEILING_ROW } },
}

local MINE_DECOR: { Decor } = {
	{ at = 16440, sheet = "mine_cart" },
	{ at = 17400, sheet = "dynamite_crate" },
	{ at = 18360, sheet = "mine_cart_keg" },
	{ at = 19320, sheet = "dynamite_crate" },
	{ at = 20280, sheet = "mine_cart" },
}

local UP_THE_MESA: { Wave } = {
	{ at = 21300, sheet = "vultureling", rows = { 260, 400, 540 } },
	{ at = 21480, sheet = "carrion", rows = { 300, 540 } },
	{ at = 21720, sheet = "cricket", rows = SWARM_HIGH },
	{ at = 21960, sheet = "vultureling", rows = { 300, 440 } },
	{ at = 22200, sheet = "carrion", rows = { 260, 460, 660 } },
	{ at = 22440, sheet = "shroud", rows = { 380 } },
	{ at = 22680, sheet = "vultureling", rows = { 260, 400, 540 } },
	{ at = 22920, sheet = "cricket", rows = SWARM_LOW },
	{ at = 23100, sheet = "vultureling", rows = { 300, 460 } },
}

level.register("deseret", {
	function()
		play(FLATS, FLATS_DECOR, 0)
		level.wait_scroll(END_OF_THE_FLATS)
		level.biome("deseret_hoodoos")
		wait_for_a_clear_sky()
	end,
	function()
		local shift = lateness(END_OF_THE_FLATS)
		play(HOODOOS, HOODOOS_DECOR, shift)
		level.wait_scroll(END_OF_THE_HOODOOS + shift)
		level.biome("deseret_line")
		wait_for_a_clear_sky()
	end,
	function()
		local shift = lateness(END_OF_THE_HOODOOS)
		play(THE_LINE, THE_LINE_DECOR, shift)
		level.wait_scroll(END_OF_THE_LINE + shift)
		wait_for_a_clear_sky()
	end,
	function()
		fight("iron_pilgrim", IRON_PILGRIM_ENTRY)
		level.biome("deseret_mine")
	end,
	function()
		local shift = lateness(END_OF_THE_LINE)
		play(MINE, MINE_DECOR, shift)
		level.wait_scroll(END_OF_THE_MINE + shift)
		level.biome("deseret_mesa")
		wait_for_a_clear_sky()
	end,
	function()
		local shift = lateness(END_OF_THE_MINE)
		play(UP_THE_MESA, {}, shift)
		level.wait_scroll(FOOT_OF_THE_MESA + shift)
		wait_for_a_clear_sky()
		fight("old_widow", OLD_WIDOW_ENTRY)
	end,
}, {
	map = "DESERET",
	preview = "deseret",
	biome = "deseret",
	starts = {
		[2] = { scroll = END_OF_THE_FLATS, biome = "deseret_hoodoos", biome_since = 0 },
		[3] = { scroll = END_OF_THE_HOODOOS, biome = "deseret_line", biome_since = 0 },
		[4] = { scroll = END_OF_THE_LINE, biome = "deseret_line", biome_since = 0 },
		[5] = { scroll = END_OF_THE_LINE, biome = "deseret_mine", biome_since = 0 },
		[6] = { scroll = END_OF_THE_MINE, biome = "deseret_mesa", biome_since = 0 },
	},
	bands = {
		deseret = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
		deseret_hoodoos = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
		deseret_line = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
		deseret_mine = { top = CEILING_ROW, bottom = FLOOR_ROW },
		deseret_mesa = { top = OPEN_SKY_ROW, bottom = FLOOR_ROW },
	},
})
