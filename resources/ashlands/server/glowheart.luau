--!strict
-- The Glowheart, the reactor of the shelter grown alive, the boss of the
-- core, and the bodies its attacks bring: the tendrils that grow out of its
-- arms and sweep the lanes, the beam of its Meltdown and the mites of its
-- Overload. Its sheet is entities/glowheart.toml.
--
-- It slides in to rest by the right edge, then plays in place its three
-- attacks in turn, each in three beats: a signal, a strike, and a recover.
-- The Tendrils grow out of two of its arms along two of the three lanes
-- through their signal, then sweep them, the third safe; the Meltdown burns its
-- row; the Overload lights its chimneys, then spits `mites` Cindermites out of
-- them (`enraged_mites` once enraged), each turning towards the nearest ship.
-- A missile hurts it anywhere on its vessel, at any time, while it attacks
-- too. The decor of the core follows it on the bus: the coolant
-- leaks and the lamps light up with the charge of the Meltdown
-- (glowheart_meltdown), and the steam vents purge with it (glowheart_purge).
--
-- Every place below is measured from the center of its body, in logical units
-- (a pixel of the sheet is 4 units), the boss resting at {rest_x, 520}.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }

local TENDRILS, OVERLOAD, MELTDOWN, ARRIVAL, RAGE = 0, 1, 2, 3, 4

-- The signal and the strike of each attack, in milliseconds; the recover of
-- the Meltdown is the purge, from the sheet.
local BEATS = {
	[TENDRILS] = { signal = 1200, strike = 900, recover = 360 },
	[MELTDOWN] = { signal = 1400, strike = 900 },
	[OVERLOAD] = { signal = 1000, strike = 600, recover = 360 },
}

-- The order it plays its attacks in, from the first.
local ROTATION = { TENDRILS, MELTDOWN, OVERLOAD }

-- The row of its core, from the center of the body.
local CORE_ROW = -12

-- The three lanes the tendrils sweep, the sheet of the tendril of each, the
-- row of its center and how far left of the center of the vessel it stands.
-- Each tendril grows out of the tip of an arm, which stays as it rests through
-- the attack (tips at {-136, 368}, {-176, 416} and {-152, 696} from the
-- vessel's left, its center row 520): its frame reaches from its lane to that
-- tip, so its center lies off its lane, and its 1600 units end at the tip, so
-- it reaches the left edge.
local LANES = {
	{ sheet = "glowheart_tendril_high", center = 334, x = -936 },
	{ sheet = "glowheart_tendril_middle", center = 468, x = -976 },
	{ sheet = "glowheart_tendril_low", center = 718, x = -952 },
}

-- Where the mites of its Overload leave its two chimneys.
local CHIMNEYS = { { x = -20, y = -228 }, { x = 106, y = -228 } }

-- Where the beam of the Meltdown lies: its 1600 units end at the vessel, on
-- the row of the core.
local BEAM = { x = -892, y = CORE_ROW }

local function from(center, place)
	return { x = center.x + place.x, y = center.y + place.y }
end

local function sweep_tendrils(center)
	local safe = math.random(1, #LANES)
	for index, lane in LANES do
		if index ~= safe then
			behaviours.spawn(lane.sheet, { x = center.x + lane.x, y = lane.center })
		end
	end
end

-- Spits `count` mites, one chimney after the other, `gap_ms` apart; returns
-- how many milliseconds it waited.
local function overload(center, count: number, gap_ms: number): number
	for mite = 1, count do
		behaviours.spawn("glowheart_mite", from(center, CHIMNEYS[(mite - 1) % #CHIMNEYS + 1]))
		if mite < count then
			behaviours.wait(gap_ms, { direction = STILL })
		end
	end
	return (count - 1) * gap_ms
end

behaviours.register("glowheart_core", {
	{ name = "rest_x", lowest = 0, highest = 1920 },
	{ name = "slide_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
	{ name = "purge_ms", lowest = 1, highest = 60000 },
	{ name = "mites", lowest = 1, highest = 16 },
	{ name = "enraged_mites", lowest = 1, highest = 16 },
	{ name = "mite_gap_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
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
		local chosen = ROTATION[(played - 1) % #ROTATION + 1]
		local beats = BEATS[chosen]

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "signal" } })
		if chosen == TENDRILS then
			sweep_tendrils(center)
		elseif chosen == MELTDOWN then
			bus.emit("glowheart_meltdown", { ms = beats.signal })
			behaviours.spawn("meltdown_beam", from(center, BEAM))
		end
		behaviours.wait(beats.signal, { direction = STILL })

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		local waited = 0
		if chosen == OVERLOAD then
			waited = overload(center, if raged then settings.enraged_mites else settings.mites, settings.mite_gap_ms)
		end
		behaviours.wait(math.max(1, beats.strike - waited), { direction = STILL })

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
