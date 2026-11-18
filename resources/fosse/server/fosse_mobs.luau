--!strict
-- The behaviours of the mobs of the Pit: every one of them crosses the
-- playfield from the right to the left, some along a curve, and each attacks
-- in three beats the client draws from its sheet (signal, strike, recover).
-- Their sheets are in entities/, their sprites in sprites/fosse/. The view and
-- the intent are described on the page "Scripting enemies in Luau" of the docs.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }
local FULL_TURN = 2 * math.pi

-- How far below the playfield a body that fell is removed, in logical units.
local BELOW_THE_PLAYFIELD = 1240

-- Whether a body is rested: idle, and idle for at least rest_ms.
local function rested(e, rest_ms: number): boolean
	return e.action == nil and e.action_age_ms >= rest_ms
end

-- Plays the three beats of attack 0, the body wanting `still` while it signals
-- and recovers; `strike` is the intent of the strike, `on_strike` what the body
-- does as it strikes. The body is idle again once it returns.
local function attack(beats, strike, on_strike: (() -> ())?)
	behaviours.wait(beats.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	if on_strike ~= nil then
		on_strike()
	end
	strike.starts_action = { attack = 0, phase = "strike" }
	behaviours.wait(beats.strike_ms, strike)
	behaviours.wait(beats.recover_ms, { direction = STILL, starts_action = { attack = 0, phase = "recover" } })
	behaviours.wait(1, { direction = LEFT, ends_action = true })
end

-- How long a body at `speed` units per second takes to cross `distance`
-- units, in milliseconds: one tick at least.
local function crossing_ms(distance: number, speed: number): number
	return math.max(1, math.abs(distance) * 1000 / speed)
end

local ATTACK_SETTINGS = {
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "strike_ms", lowest = 1, highest = 60000 },
	{ name = "recover_ms", lowest = 1, highest = 60000 },
}

local function with_attack(settings)
	for _, setting in ATTACK_SETTINGS do
		table.insert(settings, setting)
	end
	return settings
end

-- The Sporecap walks left along the floor; when a ship flies above it, within
-- reach on the x axis, its cap swells and it puffs spores up, spore_gap units
-- apart, which drift left in a wave.
behaviours.register("sporecap_walk", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
	{ name = "spores", lowest = 1, highest = 8 },
	{ name = "spore_gap", lowest = 0, highest = 540 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if
		ship ~= nil
		and rested(e, settings.rest_ms)
		and ship.y < e.position.y
		and math.abs(ship.x - e.position.x) <= settings.reach
	then
		attack(settings, { direction = STILL }, function()
			for spore = 1, settings.spores do
				behaviours.spawn("sporecap_spore", {
					x = e.position.x,
					y = e.position.y - spore * settings.spore_gap,
				})
			end
		end)
	end
	return { direction = LEFT }
end)

-- The Lanternfin swims left along a wave of `curve` units every curve_ms,
-- drawing towards the row of the nearest ship; once that ship is ahead of it,
-- within reach and level with it within align units, its lure flashes, it
-- lunges left at lunge_speed, then slows down.
behaviours.register("lanternfin_hunt", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
	{ name = "align", lowest = 0, highest = 1080 },
	{ name = "cruise_speed", lowest = 1, highest = 4096 },
	{ name = "lunge_speed", lowest = 1, highest = 4096 },
	{ name = "curve", lowest = 0, highest = 100 },
	{ name = "curve_ms", lowest = 1, highest = 60000 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	local sway = settings.curve / 100 * math.cos(FULL_TURN * e.age_ms / settings.curve_ms)
	if ship == nil then
		return { direction = { x = -1, y = sway }, top_speed = settings.cruise_speed }
	end
	local gap = ship.y - e.position.y
	local ahead = ship.x < e.position.x and e.position.x - ship.x <= settings.reach
	if ahead and math.abs(gap) <= settings.align and rested(e, settings.rest_ms) then
		attack(settings, { direction = LEFT, top_speed = settings.lunge_speed })
		return { direction = LEFT, top_speed = settings.cruise_speed }
	end
	local towards = math.clamp(gap / settings.reach, -0.5, 0.5)
	return { direction = { x = -1, y = sway + towards }, top_speed = settings.cruise_speed }
end)

-- The Stalagmaw crawls left along the ceiling at crawl_speed; when a ship
-- passes under it, within reach on the x axis, it shakes, then falls at
-- fall_speed and is removed once below the playfield. Both speeds are asked
-- for in the intent: a top speed asked for stays until another one is.
behaviours.register("stalagmaw_drop", {
	{ name = "reach", lowest = 0, highest = 1920 },
	{ name = "crawl_speed", lowest = 1, highest = 4096 },
	{ name = "fall_speed", lowest = 1, highest = 4096 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if ship == nil or ship.y <= e.position.y or math.abs(ship.x - e.position.x) > settings.reach then
		return { direction = LEFT, top_speed = settings.crawl_speed }
	end
	behaviours.wait(settings.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	behaviours.wait(crossing_ms(BELOW_THE_PLAYFIELD - e.position.y, settings.fall_speed), {
		direction = { x = 0, y = 1 },
		top_speed = settings.fall_speed,
		starts_action = { attack = 0, phase = "strike" },
	})
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- The Ruin warden floats left, bobbing `bob` units around its row every
-- bob_ms; once in the playfield and rested, its rune lights up and it fires
-- one bolt of its gun, then its rune dims.
behaviours.register("warden_float", with_attack({
	{ name = "bob", lowest = 0, highest = 100 },
	{ name = "bob_ms", lowest = 1, highest = 60000 },
	{ name = "fire_from_x", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	local bob = settings.bob / 100 * math.sin(FULL_TURN * e.age_ms / settings.bob_ms)
	if e.position.x <= settings.fire_from_x and rested(e, settings.rest_ms) then
		attack(settings, { direction = STILL, shooting = true })
	end
	return { direction = { x = -1, y = bob } }
end)
