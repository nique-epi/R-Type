--!strict
-- The Sandmaw, the worm that lives under the highway and closes Highway Zero.
-- Its sheet is entities/sandmaw.toml.
--
-- It waits under the floor, out of sight, then marks the column of the
-- nearest ship: the sand bulges there (its jaws show above the floor, the
-- signal), it bursts straight up (the strike), arcs over to the left (the
-- recover) and dives back under the floor. Its arrival is its first burst,
-- from the middle of the highway; once enraged it waits less under the floor.
-- A missile hurts it whenever it shows.

local STILL = { x = 0, y = 0 }
local UP = { x = 0, y = -1 }
local DOWN = { x = 0, y = 1 }
local ARC = { x = -0.6, y = 0.4 }

local RAGE, ARRIVAL = 1, 2

-- Where its center waits under the floor: past the bottom edge of the
-- playfield (1080) by more than half its 152-unit body.
local UNDER_THE_FLOOR = 1260

-- Where its center stands while its jaws show above the floor of the
-- highway (row 896): its top 54 units above it.
local BULGE_ROW = 918

-- The columns it may burst out of, so its whole body stays on screen, and the
-- one its arrival bursts out of.
local FIRST_COLUMN, LAST_COLUMN = 240, 1680
local MIDDLE_COLUMN = 960

local function speed_over(distance: number, ms: number): number
	return math.max(1, distance * 1000 / ms)
end

-- One burst out of the floor at `column`, as the three beats of `attack`:
-- the bulge, the strike straight up, the arc, then the dive. Returns the
-- handle as the dive ends.
local function burst(settings, column: number, attack: number)
	behaviours.wait(settings.signal_ms, {
		direction = STILL,
		jumps_to = { x = column, y = BULGE_ROW },
		starts_action = { attack = attack, phase = "signal" },
	})
	local rise = speed_over(settings.rise_height, settings.rise_ms)
	behaviours.wait(settings.rise_ms, { direction = UP, top_speed = rise, starts_action = { attack = attack, phase = "strike" } })
	local e = behaviours.wait(settings.recover_ms, { direction = ARC, top_speed = rise, starts_action = { attack = attack, phase = "recover" } })
	behaviours.wait(settings.dive_ms, {
		direction = DOWN,
		top_speed = speed_over(UNDER_THE_FLOOR - e.position.y, settings.dive_ms),
		ends_action = true,
	})
	return behaviours.wait(1, { direction = STILL, jumps_to = { x = column, y = UNDER_THE_FLOOR } })
end

behaviours.register("sandmaw_hunt", {
	{ name = "burrow_ms", lowest = 1, highest = 60000 },
	{ name = "enraged_burrow_ms", lowest = 1, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rise_height", lowest = 1, highest = 1080 },
	{ name = "recover_ms", lowest = 1, highest = 60000 },
	{ name = "dive_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	behaviours.wait(settings.burrow_ms, { direction = STILL, jumps_to = { x = MIDDLE_COLUMN, y = UNDER_THE_FLOOR } })
	e = burst(settings, MIDDLE_COLUMN, ARRIVAL)

	local raged = false
	while true do
		if e.enraged and not raged then
			raged = true
			behaviours.wait(settings.burrow_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
		end
		local ship = e.nearest_ship
		local column = math.clamp(if ship ~= nil then ship.x else MIDDLE_COLUMN, FIRST_COLUMN, LAST_COLUMN)
		behaviours.wait(if raged then settings.enraged_burrow_ms else settings.burrow_ms, { direction = STILL })
		e = burst(settings, column, 0)
	end
end)
