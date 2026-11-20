--!strict
-- The behaviours of the mobs of the Ashlands and of what they fire: every mob
-- crosses the playfield from the right to the left and attacks in three beats
-- the client draws from its sheet (signal, strike, recover). Their sheets are
-- in entities/, their sprites in sprites/cendres/. The view and the intent are
-- described on the page "Scripting enemies in Luau" of the docs.
--
-- A missile a script steers is steered once when its sheet is read, with no
-- ship and no body behind the handle: its function then reads only its
-- settings and its age, and heads left, the edge that removes it.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }
local FULL_TURN = 2 * math.pi

-- How far ahead of a mob, in logical units, the shot it fires appears.
local MUZZLE_GAP = 60

-- The right edge of the playfield: a mob fires only once it shows.
local RIGHT_EDGE = 1920

-- How long a tick lasts, in seconds: a body that keeps to the ground moves
-- up or down at most its top speed over a tick.
local TICK_SECONDS = 1 / 60

-- How far the center of a walker stands above the floor (negative) or below
-- the ceiling (positive), the gaps ashlands.luau spawns them with.
local DUNECRAWLER_GAP = -48
local FIXBOT_GAP = -56
local RAIL_TURRET_GAP = 0

-- The direction that keeps a body `gap` units from the floor of the biome the
-- level shows (or from its ceiling) while it moves left at its top speed,
-- read with level.ground (level.ceiling) under its column. It moves straight
-- left where the biome has no ground, or in a game that plays no level.
local function along(e, read: (number) -> number?, gap: number)
	local found, row = pcall(read, e.position.x)
	if not found or row == nil then
		return LEFT
	end
	local step = (row + gap - e.position.y) / (e.top_speed * TICK_SECONDS)
	return { x = -1, y = math.clamp(step, -1, 1) }
end

local function percent(value: number): number
	return value / 100
end

-- Whether a body is rested: idle, and idle for at least rest_ms.
local function rested(e, rest_ms: number): boolean
	return e.action == nil and e.action_age_ms >= rest_ms
end

-- Plays the three beats of attack 0, the body still: the signal, then the
-- strike, during which `on_strike` creates what the body fires and returns
-- how many milliseconds it waited, then the recover. The body is idle again
-- once it returns.
local function attack(beats, on_strike: () -> number)
	behaviours.wait(beats.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	behaviours.wait(1, { direction = STILL, starts_action = { attack = 0, phase = "strike" } })
	local waited = on_strike()
	behaviours.wait(math.max(1, beats.strike_ms - waited), { direction = STILL })
	behaviours.wait(beats.recover_ms, { direction = STILL, starts_action = { attack = 0, phase = "recover" } })
	behaviours.wait(1, { direction = LEFT, ends_action = true })
end

local ATTACK_SETTINGS = {
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "strike_ms", lowest = 1, highest = 60000 },
	{ name = "recover_ms", lowest = 1, highest = 60000 },
}

-- The direction of a body that flies left, bobbing `bob` percent of its
-- speed around its row every bob_ms.
local function bobbing_left(e)
	local settings = e.settings
	return { x = -1, y = percent(settings.bob) * math.sin(FULL_TURN * e.age_ms / settings.bob_ms) }
end

local function with_attack(settings)
	for _, setting in ATTACK_SETTINGS do
		table.insert(settings, setting)
	end
	return settings
end

local function fire(sheet: string, from): () -> number
	return function(): number
		behaviours.spawn(sheet, from)
		return 0
	end
end

-- The Dunecrawler walks left along the floor, keeping to it; when a ship flies above it,
-- within reach on the x axis, its tail quivers and it fires a barb straight up.
behaviours.register("dunecrawler_walk", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if
		ship ~= nil
		and rested(e, settings.rest_ms)
		and ship.y < e.position.y
		and math.abs(ship.x - e.position.x) <= settings.reach
	then
		attack(settings, fire("dunecrawler_barb", { x = e.position.x, y = e.position.y - MUZZLE_GAP }))
	end
	return { direction = along(e, level.ground, DUNECRAWLER_GAP) }
end)

-- A missile that rises, drifting left by `drift` percent of its speed.
behaviours.register("rising_barb", {
	{ name = "drift", lowest = 1, highest = 100 },
}, function(e)
	return { direction = { x = -percent(e.settings.drift), y = -1 } }
end)

-- A missile that falls, drifting left by `drift` percent of its speed.
behaviours.register("falling_shot", {
	{ name = "drift", lowest = 1, highest = 100 },
}, function(e)
	return { direction = { x = -percent(e.settings.drift), y = 1 } }
end)

-- The Scrapdrone drifts left, bobbing `bob` units around its row every
-- bob_ms; once left of fire_from_x and rested, its eye swells and it fires
-- one bolt at the nearest ship.
behaviours.register("scrapdrone_patrol", with_attack({
	{ name = "bob", lowest = 0, highest = 100 },
	{ name = "bob_ms", lowest = 1, highest = 60000 },
	{ name = "fire_from_x", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	if e.position.x <= settings.fire_from_x and rested(e, settings.rest_ms) then
		attack(settings, fire("scrapdrone_bolt", { x = e.position.x - MUZZLE_GAP, y = e.position.y }))
	end
	return { direction = bobbing_left(e) }
end)

-- A bolt aimed at the ship nearest to it the first time it sees one ahead,
-- then flying straight on; it flies left while it has seen none.
behaviours.register("aimed_bolt", {}, function(e)
	local ship = e.nearest_ship
	if ship == nil then
		return { direction = LEFT }
	end
	local data = e.data
	if data.heading == nil then
		local x, y = ship.x - e.position.x, ship.y - e.position.y
		local length = math.sqrt(x * x + y * y)
		data.heading = if x < 0 and length > 0 then { x = x / length, y = y / length } else LEFT
	end
	return { direction = data.heading }
end)

-- The Rail turret slides left along the ceiling, keeping to it; when a ship
-- flies below it, within reach on the x axis, its lens locks and it fires
-- `shots` shots down, shot_gap_ms apart.
behaviours.register("rail_turret_slide", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
	{ name = "shots", lowest = 1, highest = 8 },
	{ name = "shot_gap_ms", lowest = 1, highest = 60000 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if
		ship ~= nil
		and e.position.x < RIGHT_EDGE
		and rested(e, settings.rest_ms)
		and ship.y > e.position.y
		and math.abs(ship.x - e.position.x) <= settings.reach
	then
		local muzzle = { x = e.position.x, y = e.position.y + MUZZLE_GAP }
		attack(settings, function(): number
			for shot = 1, settings.shots do
				behaviours.spawn("rail_turret_shot", muzzle)
				if shot < settings.shots then
					behaviours.wait(settings.shot_gap_ms, { direction = STILL })
				end
			end
			return (settings.shots - 1) * settings.shot_gap_ms
		end)
	end
	return { direction = along(e, level.ceiling, RAIL_TURRET_GAP) }
end)

-- The Geiger mine drifts left, bobbing `bob` units around its row every
-- bob_ms; a ship within `trigger` units makes it tick signal_ms, then burst:
-- its sheet fires a ring of shards as the strike starts, and the mine is gone
-- once it ends. Shot before, it pops like any body.
behaviours.register("geiger_drift", {
	{ name = "trigger", lowest = 0, highest = 1920 },
	{ name = "bob", lowest = 0, highest = 100 },
	{ name = "bob_ms", lowest = 1, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "strike_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if ship ~= nil and e.position.x < RIGHT_EDGE then
		local x, y = ship.x - e.position.x, ship.y - e.position.y
		if x * x + y * y <= settings.trigger * settings.trigger then
			behaviours.wait(settings.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
			behaviours.wait(settings.strike_ms, { direction = STILL, starts_action = { attack = 0, phase = "strike" } })
			behaviours.destroy(e.id)
			return { direction = STILL }
		end
	end
	return { direction = bobbing_left(e) }
end)

-- The Fixbot rolls left on the floor, keeping to it; when a ship is ahead of it, within
-- reach, it brakes, winds its arm back and throws a wrench forward.
behaviours.register("fixbot_roll", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if
		ship ~= nil
		and e.position.x < RIGHT_EDGE
		and rested(e, settings.rest_ms)
		and ship.x < e.position.x
		and e.position.x - ship.x <= settings.reach
	then
		attack(settings, fire("fixbot_wrench", { x = e.position.x - MUZZLE_GAP, y = e.position.y - MUZZLE_GAP }))
	end
	return { direction = along(e, level.ground, FIXBOT_GAP) }
end)

-- A wrench thrown along an arc: it leaves rising at `lift` percent of its
-- speed, and its fall grows with its age until it falls as fast after
-- fall_ms, flying left all along.
behaviours.register("thrown_wrench", {
	{ name = "lift", lowest = 0, highest = 100 },
	{ name = "fall_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local lift = percent(settings.lift)
	local fall = -lift + 2 * lift * e.age_ms / settings.fall_ms
	return { direction = { x = -1, y = math.min(1, fall) } }
end)

-- A brood mite flies left, turning `turn` percent of the way towards the
-- nearest ship.
behaviours.register("brood_swarm", {
	{ name = "turn", lowest = 0, highest = 100 },
}, function(e)
	local ship = e.nearest_ship
	if ship == nil then
		return { direction = LEFT }
	end
	local x, y = ship.x - e.position.x, ship.y - e.position.y
	local length = math.max(1, math.sqrt(x * x + y * y))
	local turn = percent(e.settings.turn)
	return { direction = { x = -(1 - turn) + turn * x / length, y = turn * y / length } }
end)
