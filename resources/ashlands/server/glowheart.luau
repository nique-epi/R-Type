--!strict
-- The Glowheart, the reactor of the shelter grown alive, the boss of the
-- core, and the bodies its attacks bring: the tendrils that sweep the lanes
-- and the beam of its Meltdown. Its sheet is entities/glowheart.toml.
--
-- It slides in to rest by the right edge, then plays in place its two
-- attacks in turn, each in three beats: a signal, a strike that always leaves
-- a way through, and a recover. Its vessel closes only while it attacks, from
-- the signal of its Tendrils to the end of their sweep, and through the burn
-- of its Meltdown: the rest of the time, its rest, the charge of the Meltdown
-- and the purge of its vents after it included, a missile hurts it on the rows
-- of its core window, judged where a missile coming from the left first
-- touches the vessel. The decor of the core follows it on the bus: the coolant
-- leaks and the lamps light up with the charge of the Meltdown
-- (glowheart_meltdown), and the steam vents purge with it (glowheart_purge).
--
-- Every place below is measured from the center of its body, in logical units
-- (a pixel of the sheet is 4 units), the boss resting at {rest_x, 520}.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }

local TENDRILS, MELTDOWN, ARRIVAL, RAGE = 0, 2, 3, 4

-- The signal and the strike of each attack, in milliseconds; the recover of
-- the Meltdown is the purge, from the sheet.
local BEATS = {
	[TENDRILS] = { signal = 1200, strike = 900, recover = 360 },
	[MELTDOWN] = { signal = 1400, strike = 900 },
}

-- The core window: the rows a hit may land on, from the center of the body.
local CORE_ROW, CORE_HALF_HEIGHT = -12, 96

-- The three lanes the tendrils sweep, and where a tendril lies: its 1600
-- units end 100 units into the vessel, so it reaches the left edge.
local LANES = { 300, 520, 740 }
local TENDRIL_X = -900

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

behaviours.register("glowheart_core", {
	{ name = "rest_x", lowest = 0, highest = 1920 },
	{ name = "slide_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
	{ name = "purge_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local closed = false
	behaviours.pre_hit(function(hit)
		if not closed and math.abs(hit.offset.y - CORE_ROW) <= CORE_HALF_HEIGHT then
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
	local chosen = MELTDOWN
	while true do
		if e.enraged and not raged then
			raged = true
			behaviours.wait(settings.rage_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
		end
		chosen = if chosen == TENDRILS then MELTDOWN else TENDRILS
		local beats = BEATS[chosen]

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "signal" } })
		if chosen == TENDRILS then
			closed = true
			sweep_tendrils(center)
		else
			bus.emit("glowheart_meltdown", { ms = beats.signal })
			behaviours.spawn("meltdown_beam", from(center, BEAM))
		end
		behaviours.wait(beats.signal, { direction = STILL })

		closed = true
		behaviours.wait(beats.strike, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		closed = false

		local recover = beats.recover or settings.purge_ms
		if chosen == MELTDOWN then
			bus.emit("glowheart_purge", { ms = recover })
		end
		behaviours.wait(recover, { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)
