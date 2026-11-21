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

-- The blast doors on screen, by the network id of their upper half: the
-- center of that half, the row of the gap below it, and the scroll when the
-- door last wrote them. A door writes itself every tick (blast_door_hold), so
-- an entry a scroll older than DOOR_STALE_SCROLL, or newer than the scroll (a
-- restart from a checkpoint brought the scroll back), is a door that is gone.
local DOORS: { [number]: { x: number, gap: number, scroll: number } } = {}
local DOOR_STALE_SCROLL = 16

-- How far below the center of the upper half the middle of the gap lies: half
-- the gap (340 units) and half the 480 units of a half, as ashlands.luau lays
-- them; the room a body has around that middle; how far from a door a body
-- heads for its gap.
local DOOR_REACH = 410
local GAP_HALF = 170
local GAP_MARGIN = 8
local DOOR_LOOKAHEAD = 720

-- Half the width of a door, and how close behind a body walking on the floor
-- or the ceiling a door may come before the body stops attacking and runs
-- ahead of it: every walker is faster than the scroll that carries the door.
local DOOR_HALF_WIDTH = 44
local DOOR_GUARD = 480

-- The door nearest to a body within DOOR_LOOKAHEAD, ahead of it or behind it:
-- a door scrolls left, so it catches up with a body slower than the scroll, or
-- one that stands still to attack; nil when there is none.
local function door_ahead(e)
	local now = level.scroll()
	local found = nil
	for id, door in DOORS do
		local age = now - door.scroll
		if age < 0 or age > DOOR_STALE_SCROLL then
			DOORS[id] = nil
		else
			local x = door.x - age
			local away = math.abs(e.position.x - x)
			if away < DOOR_LOOKAHEAD and (found == nil or away < found.away) then
				found = { x = x, gap = door.gap, away = away }
			end
		end
	end
	return found
end

-- The direction of a body that keeps `direction` but, while a blast door is
-- near it, heads for the middle of its gap, so the door passes it at its gap
-- and never through it.
local function through_doors(e, direction)
	local door = door_ahead(e)
	if door == nil then
		return direction
	end
	local room = math.max(0, GAP_HALF - e.collision_box.height / 2 - GAP_MARGIN)
	local offset = e.position.y - door.gap
	if math.abs(offset) <= room and math.abs(direction.y) * e.top_speed * TICK_SECONDS <= room - math.abs(offset) then
		return direction
	end
	local step = -offset / (e.top_speed * TICK_SECONDS)
	return { x = direction.x, y = math.clamp(step, -1, 1) }
end

-- The direction of a body that walks on the floor or the ceiling, which a
-- door covers whatever its gap: it keeps `direction`, but never walks into a
-- door ahead of it; it waits behind it until the scroll takes the door away.
local function behind_doors(e, direction)
	local door = door_ahead(e)
	if door == nil or door.x > e.position.x then
		return direction
	end
	local room = e.position.x - door.x - DOOR_HALF_WIDTH - e.collision_box.width / 2
	local most = math.max(-1, -math.max(0, room) / (e.top_speed * TICK_SECONDS))
	return { x = math.max(direction.x, most), y = direction.y }
end

-- Whether a door comes behind a walker, close enough to run it over while it
-- stands still to attack.
local function door_behind(e): boolean
	local door = door_ahead(e)
	return door ~= nil and door.x > e.position.x and door.x - e.position.x < DOOR_GUARD
end

-- The upper half of a blast door stands on the scroll and writes where it is
-- and where its gap lies, for the mobs to fly through it.
behaviours.register("blast_door_hold", {}, function(e)
	DOORS[e.id] = { x = e.position.x, gap = e.position.y + DOOR_REACH, scroll = level.scroll() }
	return { direction = STILL }
end)

-- The Cindermite flies left at its top speed, weaving `amplitude` units around
-- its row every period_ms; a blast door ahead moves that row to its gap.
behaviours.register("cindermite_weave", {
	{ name = "amplitude", lowest = 0, highest = 48 },
	{ name = "period_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	e.data.row = e.data.row or e.position.y
	local door = door_ahead(e)
	if door ~= nil then
		local reach = e.top_speed * TICK_SECONDS
		e.data.row += math.clamp(door.gap - e.data.row, -reach, reach)
	end
	local target = e.data.row + settings.amplitude * math.sin(FULL_TURN * e.age_ms / settings.period_ms)
	local step = (target - e.position.y) / (e.top_speed * TICK_SECONDS)
	return { direction = { x = -1, y = math.clamp(step, -1, 1) } }
end)

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
		and not door_behind(e)
		and ship.y < e.position.y
		and math.abs(ship.x - e.position.x) <= settings.reach
	then
		attack(settings, fire("dunecrawler_barb", { x = e.position.x, y = e.position.y - MUZZLE_GAP }))
	end
	return { direction = behind_doors(e, along(e, level.ground, DUNECRAWLER_GAP)) }
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
	return { direction = through_doors(e, bobbing_left(e)) }
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
		and not door_behind(e)
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
	return { direction = behind_doors(e, along(e, level.ceiling, RAIL_TURRET_GAP)) }
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
	return { direction = through_doors(e, bobbing_left(e)) }
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
		and not door_behind(e)
		and ship.x < e.position.x
		and e.position.x - ship.x <= settings.reach
	then
		attack(settings, fire("fixbot_wrench", { x = e.position.x - MUZZLE_GAP, y = e.position.y - MUZZLE_GAP }))
	end
	return { direction = behind_doors(e, along(e, level.ground, FIXBOT_GAP)) }
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
-- nearest ship, and through the gap of a blast door ahead.
behaviours.register("brood_swarm", {
	{ name = "turn", lowest = 0, highest = 100 },
}, function(e)
	local ship = e.nearest_ship
	if ship == nil then
		return { direction = through_doors(e, LEFT) }
	end
	local x, y = ship.x - e.position.x, ship.y - e.position.y
	local length = math.max(1, math.sqrt(x * x + y * y))
	local turn = percent(e.settings.turn)
	return { direction = through_doors(e, { x = -(1 - turn) + turn * x / length, y = turn * y / length }) }
end)
