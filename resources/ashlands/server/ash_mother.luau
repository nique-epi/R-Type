--!strict
-- The Ash Mother, the queen every cindermite of the Ashlands came from, the
-- final boss of Silo Zero, and the bodies her attacks bring: the beam of her
-- Atomic breath, the brood mites of her egg sac and the eggs her legs fling.
-- Her sheet is entities/ash_mother.toml.
--
-- She waits below the screen through her warning, then appears where she
-- rests and climbs out of the rubble of the silo (her arrival, whose first
-- frames are cut at the top of the mound, and during which no hit counts),
-- then plays in place her four attacks, each in three beats: the Scythes slam
-- across the lower front, the Atomic breath charges then burns her row, the
-- Brood pours mites towards the ships, and the Fling throws eggs with her
-- front legs at where the nearest ship was AIM_BEFORE_MS before the throw,
-- spread around that point, along a low arc that never follows the ship. Her
-- head, thorax, abdomen, spikes and wings take damage wherever a missile
-- touches them, except while she strikes; her legs have no box, so a missile
-- flies through them. Below 40 % of her health she spreads her wings (attack
-- 4): she rests less, and each brood and each fling holds more.
--
-- Every place below is measured from the center of her body, in logical units
-- (a pixel of the sheet is 4 units), her frame standing on the floor of the
-- silo.

local STILL = { x = 0, y = 0 }
local LEFT = { x = -1, y = 0 }

local SCYTHES, BREATH, BROOD, ARRIVAL, RAGE, FLING = 0, 1, 2, 3, 4, 5

-- The beats of each attack, in milliseconds, the lengths of her animations
-- held longer where the players need time to read them.
local BEATS = {
	[SCYTHES] = { signal = 800, strike = 240, recover = 360 },
	[BREATH] = { signal = 1200, strike = 600, recover = 360 },
	[BROOD] = { signal = 800, strike = 240, recover = 360 },
	[FLING] = { signal = 700, strike = 240, recover = 360 },
	[ARRIVAL] = { strike = 240, recover = 360 },
}

-- The row of her maw, from the center of her body.
local MAW_ROW = 64

-- Where the beam of her breath lies: its 1600 units end at her maw.
local BEAM = { x = -990, y = MAW_ROW }

-- Where the mites pour out of her egg sac, and how far apart they leave.
local SAC = { x = 120, y = -40 }
local MITE_GAP = 56

-- Where her front legs fling the eggs from: the tips of her scythes, raised;
-- how long before the throw she takes the place of the nearest ship, and how
-- far apart her eggs land around it.
local LEGS = { x = -200, y = -120 }
local AIM_BEFORE_MS = 500
local EGG_SPREAD = 120

-- Where an egg not yet flying is to land, in the order they were flung: an
-- egg takes the first on its first tick.
local egg_targets: { { x: number, y: number } } = {}

local function from(center, place)
	return { x = center.x + place.x, y = center.y + place.y }
end

local function pour_brood(center, count: number)
	local first = -(count - 1) * MITE_GAP / 2
	for mite = 0, count - 1 do
		behaviours.spawn("brood_mite", from(center, { x = SAC.x, y = SAC.y + first + mite * MITE_GAP }))
	end
end

-- Flings `count` eggs at `target`, spread EGG_SPREAD apart around it, gap_ms
-- apart, and returns the milliseconds it waited.
local function fling_eggs(center, target, count: number, gap_ms: number): number
	for egg = 1, count do
		local spread = (egg - (count + 1) / 2) * EGG_SPREAD
		table.insert(egg_targets, { x = target.x + spread, y = target.y })
		behaviours.spawn("ash_egg", from(center, LEGS))
		if egg < count then
			behaviours.wait(gap_ms, { direction = STILL })
		end
	end
	return (count - 1) * gap_ms
end

-- The order of her attacks: Scythes, Fling, Breath, Brood first, then each
-- drawn at random.
local FIRST_ORDER = { SCYTHES, FLING, BREATH, BROOD }
local ATTACKS = { SCYTHES, BREATH, BROOD, FLING }

local function next_attack(played: number): number
	return FIRST_ORDER[played] or ATTACKS[math.random(1, #ATTACKS)]
end

behaviours.register("ash_mother_brood", {
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
	{ name = "brood", lowest = 1, highest = 16 },
	{ name = "enraged_brood", lowest = 1, highest = 16 },
	{ name = "eggs", lowest = 1, highest = 16 },
	{ name = "enraged_eggs", lowest = 1, highest = 16 },
	{ name = "egg_gap_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local striking = true
	behaviours.pre_hit(function(hit)
		if not striking then
			return true
		end
		behaviours.destroy(hit.missile)
		return false
	end)

	behaviours.wait(settings.rise_ms, {
		direction = STILL,
		jumps_to = { x = e.position.x, y = settings.rest_y },
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	behaviours.wait(BEATS[ARRIVAL].strike, { direction = STILL, starts_action = { attack = ARRIVAL, phase = "strike" } })
	behaviours.wait(BEATS[ARRIVAL].recover, { direction = STILL, starts_action = { attack = ARRIVAL, phase = "recover" } })
	e = behaviours.wait(1, { direction = STILL, ends_action = true })
	local center = { x = e.position.x, y = e.position.y }
	striking = false

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
			behaviours.spawn("atomic_breath", from(center, BEAM))
		end
		local target = nil
		if chosen == FLING then
			e = behaviours.wait(beats.signal - AIM_BEFORE_MS, { direction = STILL })
			local ship = e.nearest_ship
			target = if ship ~= nil then { x = ship.x, y = ship.y } else from(center, BEAM)
			behaviours.wait(AIM_BEFORE_MS, { direction = STILL })
		else
			behaviours.wait(beats.signal, { direction = STILL })
		end

		striking = true
		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		local waited = 0
		if chosen == BROOD then
			pour_brood(center, if raged then settings.enraged_brood else settings.brood)
		elseif chosen == FLING then
			waited = fling_eggs(center, target, if raged then settings.enraged_eggs else settings.eggs, settings.egg_gap_ms)
		end
		behaviours.wait(math.max(1, beats.strike - waited), { direction = STILL })
		striking = false

		behaviours.wait(beats.recover, { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)

-- An egg flung by the Ash Mother: on its first tick it takes the place it is
-- to land on, then flies there in flight_ms along a line bent up by a bump of
-- `bump` units at its middle, and falls on past it along the same curve until
-- it leaves the screen. It flies left while it has no place, as it does when
-- its sheet is read.
behaviours.register("lobbed_egg", {
	{ name = "flight_ms", lowest = 1, highest = 60000 },
	{ name = "bump", lowest = 0, highest = 1080 },
}, function(e)
	if #egg_targets == 0 and e.nearest_ship == nil then
		return { direction = LEFT }
	end
	local settings = e.settings
	local data = e.data
	local seconds = settings.flight_ms / 1000
	if data.across == nil then
		local target = table.remove(egg_targets, 1) or e.nearest_ship
		data.across = (target.x - e.position.x) / seconds
		data.down = (target.y - e.position.y) / seconds
		data.since_ms = e.age_ms
	end
	local share = (e.age_ms - data.since_ms) / settings.flight_ms
	local bend = -4 * settings.bump * (1 - 2 * share) / seconds
	local up = data.down + bend
	local top = math.max(math.abs(data.across), math.abs(up), 1)
	return { direction = { x = data.across / top, y = up / top }, top_speed = top }
end)
