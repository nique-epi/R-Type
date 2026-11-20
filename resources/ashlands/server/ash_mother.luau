--!strict
-- The Ash Mother, the queen every cindermite of the Ashlands came from, the
-- final boss of Silo Zero, and the bodies her attacks bring: the beam of her
-- Atomic breath and the brood mites of her egg sac. Her sheet is
-- entities/ash_mother.toml.
--
-- She climbs out of the rubble of the silo (her arrival, during which no hit
-- counts), then plays in place her three attacks, each in three beats: the
-- Scythes slam across the lower front, the Atomic breath charges then burns
-- her row, the Brood pours mites towards the ships. The armour of her head and
-- thorax stops every missile: her maw takes damage only while it opens, from
-- the signal of the breath to the end of its recover, and only on its rows,
-- judged where a missile coming from the left first touches her head. Below
-- 40 % of her health she spreads her wings (attack 4): she rests less, and
-- each brood holds more mites.
--
-- Every place below is measured from the center of her body, in logical units
-- (a pixel of the sheet is 4 units), her frame standing on the floor of the
-- silo.

local STILL = { x = 0, y = 0 }
local UP = { x = 0, y = -1 }

local SCYTHES, BREATH, BROOD, ARRIVAL, RAGE = 0, 1, 2, 3, 4

-- The beats of each attack, in milliseconds, the lengths of her animations
-- held longer where the players need time to read them.
local BEATS = {
	[SCYTHES] = { signal = 800, strike = 240, recover = 360 },
	[BREATH] = { signal = 1200, strike = 600, recover = 360 },
	[BROOD] = { signal = 800, strike = 240, recover = 360 },
	[ARRIVAL] = { strike = 240, recover = 360 },
}

-- The rows of her open maw, from the center of her body.
local MAW_ROW, MAW_HALF_HEIGHT = 64, 56

-- Where the beam of her breath lies: its 1600 units end at her maw.
local BEAM = { x = -990, y = MAW_ROW }

-- Where the mites pour out of her egg sac, and how far apart they leave.
local SAC = { x = 120, y = -40 }
local MITE_GAP = 56

local function from(center, place)
	return { x = center.x + place.x, y = center.y + place.y }
end

local function pour_brood(center, count: number)
	local first = -(count - 1) * MITE_GAP / 2
	for mite = 0, count - 1 do
		behaviours.spawn("brood_mite", from(center, { x = SAC.x, y = SAC.y + first + mite * MITE_GAP }))
	end
end

-- The order of her attacks: Scythes, Breath, Brood first, then two drawn at
-- random before each Breath, so her maw opens every third attack.
local function next_attack(played: number): number
	if played <= 3 then
		return played - 1
	end
	if played % 3 == 2 then
		return BREATH
	end
	return if math.random(0, 1) == 0 then SCYTHES else BROOD
end

behaviours.register("ash_mother_brood", {
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
	{ name = "brood", lowest = 1, highest = 16 },
	{ name = "enraged_brood", lowest = 1, highest = 16 },
}, function(e)
	local settings = e.settings
	local maw_open = false
	behaviours.pre_hit(function(hit)
		if maw_open and math.abs(hit.offset.y - MAW_ROW) <= MAW_HALF_HEIGHT then
			return true
		end
		behaviours.destroy(hit.missile)
		return false
	end)

	local climb = math.max(1, (e.position.y - settings.rest_y) * 1000 / settings.rise_ms)
	behaviours.wait(settings.rise_ms, {
		direction = UP,
		top_speed = climb,
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	behaviours.wait(BEATS[ARRIVAL].strike, { direction = STILL, starts_action = { attack = ARRIVAL, phase = "strike" } })
	behaviours.wait(BEATS[ARRIVAL].recover, { direction = STILL, starts_action = { attack = ARRIVAL, phase = "recover" } })
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
		if chosen == BREATH then
			maw_open = true
			behaviours.spawn("atomic_breath", from(center, BEAM))
		end
		behaviours.wait(beats.signal, { direction = STILL })

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		if chosen == BROOD then
			pour_brood(center, if raged then settings.enraged_brood else settings.brood)
		end
		behaviours.wait(beats.strike, { direction = STILL })

		behaviours.wait(beats.recover, { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		maw_open = false
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)
