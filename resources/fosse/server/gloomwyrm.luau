--!strict
-- The Gloomwyrm, a serpent of bones of the Pit: a skull head (gloomwyrm), its
-- vertebrae (gloomwyrm_segment) and its tail (gloomwyrm_tail). Their sheets
-- are in entities/, their sprites in sprites/fosse/.
--
-- The head enters from the right, swims left along a wave, turns back along a
-- half circle, down when it entered in the upper half of the Pit and up
-- otherwise, and swims out to the right along a wave again. Its path is a
-- velocity for each tick of it (path_velocity), so a body that plays the same
-- ticks moves along the same curve: each vertebra plays the path of the head
-- LINK_TICKS later than the body before it, and the wyrm moves as one.
--
-- The head spawns its bodies in a line behind it, LINK_TICKS of path apart,
-- and tells each of them, by the event gloomwyrm_link, where the path starts
-- and how it bends. As the serpents of the arcade shoot'em ups do, every part
-- is shot down on its own: a few shots break a vertebra, and the wyrm swims on
-- without it; the skull is the weak point, and once it is killed every body
-- left bursts in turn, from the neck to the tail, BURST_TICKS apart. A head
-- that swims out of the playfield is removed without a kill: its bodies swim
-- out after it.

local LEFT = { x = -1, y = 0 }

-- How many ticks of path separate two bodies of the wyrm, and how long a tick
-- is, in seconds.
local LINK_TICKS = 10
local TICK_SECONDS = 1 / 60

-- The middle row of the Pit: a wyrm that enters above it turns down.
local MIDDLE_ROW = 520

-- Past the right edge, where a body of the wyrm that swims out is removed.
local EXIT_COLUMN = 2100

-- How many ticks separate the bursts of two bodies once the skull is killed,
-- and the effect each plays, in [appearance.effects] of its sheet.
local BURST_TICKS = 5
local BURST_EFFECT = "burst"

type Path = {
	speed: number,
	wave: number,
	wave_ticks: number,
	waves: number,
	radius: number,
	turn_ticks: number,
	turn: number,
}

-- The path a head of `settings` swims at `speed`, from the row `row`.
local function path_of(settings, speed: number, row: number): Path
	local wave_ticks = math.max(1, math.round(settings.wave_ms / 1000 / TICK_SECONDS))
	local turn_ticks = math.max(1, math.round(math.pi * settings.radius / speed / TICK_SECONDS))
	return {
		speed = speed,
		wave = settings.wave,
		wave_ticks = wave_ticks,
		waves = settings.waves,
		radius = settings.radius,
		turn_ticks = turn_ticks,
		turn = if row < MIDDLE_ROW then 1 else -1,
	}
end

-- The vertical speed of a wave of `path`, `tick` ticks into it: the body goes
-- `wave` units down and back each wave, and swims level at each end of one.
local function wave_velocity(path: Path, tick: number): number
	local angle = 2 * math.pi * tick / path.wave_ticks
	return path.wave / 2 * (2 * math.pi / (path.wave_ticks * TICK_SECONDS)) * math.sin(angle)
end

-- The velocity of a body `tick` ticks into `path`, in logical units per
-- second, and whether it faces right then: left along the waves, along the
-- half circle from left to right, then right along the waves. Before the path
-- starts, the body swims straight left.
local function path_velocity(path: Path, tick: number): (number, number, boolean)
	local out_ticks = path.wave_ticks * path.waves
	if tick < 0 then
		return -path.speed, 0, false
	end
	if tick < out_ticks then
		return -path.speed, wave_velocity(path, tick), false
	end
	local into_turn = tick - out_ticks
	if into_turn < path.turn_ticks then
		local angle = math.pi * (into_turn + 0.5) / path.turn_ticks
		local speed = math.pi * path.radius / (path.turn_ticks * TICK_SECONDS)
		return -speed * math.cos(angle), path.turn * speed * math.sin(angle), angle > math.pi / 2
	end
	return path.speed, wave_velocity(path, into_turn - path.turn_ticks), true
end

-- The intent of a body that moves at (`x`, `y`) units per second, facing
-- right when `right` is, for a sheet whose look `right` mirrors it.
local function moving(x: number, y: number, right: boolean, mirrored: boolean)
	local top = math.max(math.abs(x), math.abs(y), 1)
	return {
		direction = { x = x / top, y = y / top },
		top_speed = top,
		look = if mirrored and right then "right" else "",
	}
end

behaviours.register("gloomwyrm_head", {
	{ name = "wave", lowest = 0, highest = 400 },
	{ name = "wave_ms", lowest = 100, highest = 10000 },
	{ name = "waves", lowest = 1, highest = 16 },
	{ name = "radius", lowest = 40, highest = 400 },
	{ name = "length", lowest = 1, highest = 24 },
}, function(e)
	local data = e.data
	if data.path == nil then
		data.path = path_of(e.settings, e.top_speed, e.position.y)
		data.tick = 0
		data.bodies = {}
		local gap = e.top_speed * LINK_TICKS * TICK_SECONDS
		for index = 1, e.settings.length do
			local sheet = if index == e.settings.length then "gloomwyrm_tail" else "gloomwyrm_segment"
			data.bodies[index] = behaviours.spawn(sheet, { x = e.position.x + index * gap, y = e.position.y })
		end
	elseif data.tick == 1 then
		local path = data.path
		for index, body in data.bodies do
			bus.emit("gloomwyrm_link", {
				head = e.id,
				body = body,
				index = index,
				speed = path.speed,
				wave = path.wave,
				wave_ticks = path.wave_ticks,
				waves = path.waves,
				radius = path.radius,
				turn_ticks = path.turn_ticks,
				turn = path.turn,
			})
		end
	end
	local tick = data.tick
	data.tick = tick + 1
	local x, y, right = path_velocity(data.path, tick)
	if right and e.position.x > EXIT_COLUMN then
		behaviours.destroy(e.id)
	end
	return moving(x, y, right, true)
end)

-- A vertebra or the tail: it swims left until its head tells it the path,
-- then plays that path LINK_TICKS later than the body before it. Once its
-- head is killed it bursts, its index times BURST_TICKS later, and is removed
-- the tick after, so every player sees the burst.
behaviours.register("gloomwyrm_body", {}, function(e)
	local data = e.data
	if data.tick == nil then
		local own_id = e.id
		data.tick = 0
		bus.on("gloomwyrm_link", function(link)
			if link.body == own_id then
				data.head = link.head
				data.index = link.index
				data.path = link
			end
		end)
		bus.on("entity_destroyed", function(event)
			if data.head ~= nil and event.id == data.head and event.killed then
				data.burst_tick = data.tick + data.index * BURST_TICKS
			end
		end)
	end
	local tick = data.tick
	data.tick = tick + 1
	if data.burst_tick ~= nil and tick > data.burst_tick then
		behaviours.destroy(e.id)
	end
	if data.path == nil then
		return { direction = LEFT }
	end
	local x, y, right = path_velocity(data.path, tick - data.index * LINK_TICKS)
	if right and e.position.x > EXIT_COLUMN then
		behaviours.destroy(e.id)
	end
	local intent = moving(x, y, right, true)
	if tick == data.burst_tick then
		intent.effect = BURST_EFFECT
	end
	return intent
end)
