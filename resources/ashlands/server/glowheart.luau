--!strict
-- The Glowheart, the reactor of the shelter grown alive, the boss of the
-- core, and the bodies its attacks bring: the tendrils that sweep the lanes,
-- the drops of its Fallout rain and the beam of its Meltdown. Its sheet is
-- entities/glowheart.toml.
--
-- It slides in to rest by the right edge, then plays in place its three
-- attacks, each in three beats: a signal, a strike that always leaves a way
-- through, and a recover. Its vessel is armoured: a missile hurts it only
-- while its vents purge, after each Meltdown, and only on the rows of its core
-- window, judged where a missile coming from the left first touches the
-- vessel. The decor of the core follows it on the bus: the coolant leaks light
-- up with the rain (glowheart_rain), the lamps flash with the Meltdown
-- (glowheart_meltdown) and the steam vents purge with it (glowheart_purge).
--
-- Every place below is measured from the center of its body, in logical units
-- (a pixel of the sheet is 4 units), the boss resting at {rest_x, 520}.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }

local TENDRILS, RAIN, MELTDOWN, ARRIVAL, RAGE = 0, 1, 2, 3, 4

-- The signal and the strike of each attack, in milliseconds; the recover of
-- the Meltdown is the purge, from the sheet.
local BEATS = {
	[TENDRILS] = { signal = 1200, strike = 900, recover = 360 },
	[RAIN] = { signal = 1000, strike = 1200, recover = 360 },
	[MELTDOWN] = { signal = 1400, strike = 900 },
}

-- The core window: the rows a hit may land on, from the center of the body.
local CORE_ROW, CORE_HALF_HEIGHT = -12, 64

-- The three lanes the tendrils sweep, and where a tendril lies: its 1600
-- units end 100 units into the vessel, so it reaches the left edge.
local LANES = { 300, 520, 740 }
local TENDRIL_X = -900

-- The columns the drops of the rain fall from, 120 units apart, the row they
-- start from, under the ceiling, the two columns left free each time, and the
-- second wave of drops.
local FIRST_COLUMN, COLUMN_GAP, COLUMNS = 120, 120, 11
local DROP_ROW = 222
local FREE_COLUMNS = 2
local SECOND_WAVE_MS = 450

-- Where the beam of the Meltdown lies: its 1600 units end at the vessel, on
-- the row of the core.
local BEAM = { x = -892, y = CORE_ROW }

local function from(center, place)
	return { x = center.x + place.x, y = center.y + place.y }
end

local function sweep_tendrils(center)
	local safe = math.random(1, #LANES)
	for lane, row in LANES do
		if lane ~= safe then
			behaviours.spawn("glowheart_tendril", { x = center.x + TENDRIL_X, y = row })
		end
	end
end

local function drop_rain(free: number)
	for column = 0, COLUMNS - 1 do
		if column < free or column >= free + FREE_COLUMNS then
			behaviours.spawn("glowheart_drop", { x = FIRST_COLUMN + column * COLUMN_GAP, y = DROP_ROW })
		end
	end
end

-- Each returns the milliseconds of the strike it waited.
local function strike_rain(): number
	local free = math.random(0, COLUMNS - FREE_COLUMNS)
	drop_rain(free)
	behaviours.wait(SECOND_WAVE_MS, { direction = STILL })
	drop_rain(free)
	return SECOND_WAVE_MS
end

-- The order of its attacks: Tendrils, Rain, Meltdown first, then a Tendrils
-- and a Rain in either order before each Meltdown, so its core opens every
-- third attack.
local function next_attack(played: number): number
	if played <= 3 then
		return played - 1
	end
	if played % 3 == 0 then
		return MELTDOWN
	end
	return if math.random(0, 1) == 0 then TENDRILS else RAIN
end

behaviours.register("glowheart_core", {
	{ name = "rest_x", lowest = 0, highest = 1920 },
	{ name = "slide_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
	{ name = "purge_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local purging = false
	behaviours.pre_hit(function(hit)
		if purging and math.abs(hit.offset.y - CORE_ROW) <= CORE_HALF_HEIGHT then
			return true
		end
		behaviours.destroy(hit.missile)
		return false
	end)

	local slide = math.max(1, (e.position.x - settings.rest_x) * 1000 / settings.slide_ms)
	behaviours.wait(settings.slide_ms, {
		direction = LEFT,
		top_speed = slide,
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	e = behaviours.wait(1, { direction = STILL, ends_action = true })
	local center = { x = e.position.x, y = e.position.y }

	local raged = false
	local played = 0
	while true do
		if e.enraged and not raged then
			raged = true
			behaviours.wait(settings.rage_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
		end
		played += 1
		local chosen = next_attack(played)
		local beats = BEATS[chosen]

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "signal" } })
		if chosen == TENDRILS then
			sweep_tendrils(center)
		elseif chosen == RAIN then
			bus.emit("glowheart_rain", { ms = beats.signal })
		else
			bus.emit("glowheart_meltdown", { ms = beats.signal })
			behaviours.spawn("meltdown_beam", from(center, BEAM))
		end
		behaviours.wait(beats.signal, { direction = STILL })

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		local struck = if chosen == RAIN then strike_rain() else 0
		behaviours.wait(math.max(1, beats.strike - struck), { direction = STILL })

		local recover = beats.recover or settings.purge_ms
		if chosen == MELTDOWN then
			purging = true
			bus.emit("glowheart_purge", { ms = recover })
		end
		behaviours.wait(recover, { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		purging = false
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)
