--!strict
-- The Iron Pilgrim, the locomotive the Bydo grew into on the Line, and the
-- cinders of its volley (pilgrim_cinder). Its sheet is
-- entities/iron_pilgrim.toml.
--
-- It rolls in from past the right edge to its rest on the track, in the
-- right third of the screen, then takes its attacks in turn, rested between
-- two: the volley (attack 0), its stack smoking then spitting cinders along
-- arcs; the charge (attack 1), its headlamp and whistle, then a rush to the
-- left along the floor, out of the screen, and back from the right 1.5 s
-- later, its cowcatcher alone hurting; the brood (attack 2), its growths
-- bursting into three Carrions; the burning coal (attack 5), its stack
-- glowing then lobbing lumps of coal around the nearest ship, each lighting
-- a fire on the floor where it lands. Once enraged it rests less and spits
-- and lobs more. Each attack plays an effect its sheet draws: the blast of
-- the stack, the flare of the headlamp, the burst of the growths, the soot
-- of the coal.
--
-- A missile that touches its boiler or its cab hurts it, at every moment of
-- the fight, its arrival and its attacks included; the stack has no box.

local STILL = { x = 0, y = 0 }
local LEFT = { x = -1, y = 0 }

local VOLLEY, CHARGE, BROOD, RAGE, ARRIVAL, COAL = 0, 1, 2, 3, 4, 5

-- The turn of its attacks, which it takes again from the start.
local TURN = { VOLLEY, COAL, CHARGE, VOLLEY, BROOD, COAL }

-- The effects of its sheet each attack plays.
local STACK_BLAST = "stack_blast"
local HEADLAMP_FLARE = "headlamp_flare"
local FLESH_BURST = "flesh_burst"
local COAL_TOSS = "coal_toss"

-- How far below its center the bottom of its wheels lies, so it rolls on
-- the ground under it.
local WHEELS_BELOW = 112

-- Where the cinders leave its stack, and where the Carrions burst from its
-- growths, from its center.
local STACK = { x = -72, y = -80 }
local GROWTHS = { { x = -120, y = -24 }, { x = -40, y = -40 }, { x = 40, y = -24 } }

-- Past the left and the right edges, where its charge leaves the screen and
-- comes back from.
local GONE_LEFT = -300
local BACK_RIGHT = 2300

-- The angles of a volley of five, then of seven, in degrees above the left.
local FIVE_ANGLES = { 20, 35, 50, 65, 80 }
local SEVEN_ANGLES = { 15, 26, 37, 48, 59, 70, 81 }

-- The angles the next cinders take, one each, in the order they appear.
local volley_angles: { number } = {}

-- How far apart the lumps of a throw of coal land, around the nearest ship;
-- the columns they land between; the row of the floor where the biome gives
-- no ground, and how far above the ground a fire stands (half its 128 units).
local COAL_GAP = 240
local COAL_LEFT, COAL_RIGHT = 120, 1260
local COAL_FLOOR_Y = 904
local FIRE_ABOVE = 64

-- Where the next lumps of coal land, one each, in the order they appear.
local coal_targets: { { x: number, y: number } } = {}

local function rest_row(e, settings): number
	local found, ground = pcall(level.ground, settings.rest_x)
	if not found or ground == nil then
		return settings.rest_y
	end
	return ground - WHEELS_BELOW
end

local function beat(attack: number, phase: string, ms: number, effect: string?)
	behaviours.wait(ms, { direction = STILL, starts_action = { attack = attack, phase = phase }, effect = effect })
end

local function ground_at(x: number): number
	local found, ground = pcall(level.ground, x)
	if not found or ground == nil then
		return COAL_FLOOR_Y
	end
	return ground
end

local function volley(settings, raged: boolean)
	beat(VOLLEY, "signal", settings.volley_signal_ms)
	local angles = if raged then SEVEN_ANGLES else FIVE_ANGLES
	local e = behaviours.wait(1, {
		direction = STILL,
		starts_action = { attack = VOLLEY, phase = "strike" },
		effect = STACK_BLAST,
	})
	for index = 1, if raged then settings.enraged_cinders else settings.cinders do
		table.insert(volley_angles, angles[(index - 1) % #angles + 1])
		behaviours.spawn("pilgrim_cinder", { x = e.position.x + STACK.x, y = e.position.y + STACK.y })
	end
	behaviours.wait(settings.volley_strike_ms, { direction = STILL })
	beat(VOLLEY, "recover", settings.volley_recover_ms)
end

-- The charge: it rushes left along the floor until it left the screen, waits
-- there charge_away_ms, then rolls back from the right edge to its rest.
local function charge(e, settings, rest: { x: number, y: number })
	beat(CHARGE, "signal", settings.charge_signal_ms, HEADLAMP_FLARE)
	e = behaviours.wait(1, { direction = LEFT, starts_action = { attack = CHARGE, phase = "strike" } })
	while e.position.x > GONE_LEFT do
		e = behaviours.wait(1, { direction = LEFT })
	end
	behaviours.wait(settings.charge_away_ms, { direction = STILL, jumps_to = { x = BACK_RIGHT, y = rest.y } })
	e = behaviours.wait(1, { direction = LEFT })
	while e.position.x > rest.x do
		e = behaviours.wait(1, { direction = LEFT })
	end
	behaviours.wait(1, { direction = STILL, jumps_to = rest })
	beat(CHARGE, "recover", settings.charge_recover_ms)
end

local function brood(settings)
	beat(BROOD, "signal", settings.brood_signal_ms)
	local e = behaviours.wait(1, {
		direction = STILL,
		starts_action = { attack = BROOD, phase = "strike" },
		effect = FLESH_BURST,
	})
	for index = 1, settings.brood do
		local growth = GROWTHS[(index - 1) % #GROWTHS + 1]
		behaviours.spawn("carrion", { x = e.position.x + growth.x, y = e.position.y + growth.y })
	end
	behaviours.wait(settings.brood_strike_ms, { direction = STILL })
	beat(BROOD, "recover", settings.brood_recover_ms)
end

-- The burning coal: its lumps land COAL_GAP apart, centered on the nearest
-- ship (on the middle of the floor when none is in play), between COAL_LEFT
-- and COAL_RIGHT, so a ship moves away from where it was to dodge them.
local function coal(settings, raged: boolean)
	beat(COAL, "signal", settings.coal_signal_ms)
	local e = behaviours.wait(1, {
		direction = STILL,
		starts_action = { attack = COAL, phase = "strike" },
		effect = COAL_TOSS,
	})
	local count = if raged then settings.enraged_coals else settings.coals
	local center = if e.nearest_ship ~= nil then e.nearest_ship.x else (COAL_LEFT + COAL_RIGHT) / 2
	for index = 1, count do
		local x = math.clamp(center + (index - (count + 1) / 2) * COAL_GAP, COAL_LEFT, COAL_RIGHT)
		table.insert(coal_targets, { x = x, y = ground_at(x) })
		behaviours.spawn("pilgrim_coal", { x = e.position.x + STACK.x, y = e.position.y + STACK.y })
	end
	behaviours.wait(settings.coal_strike_ms, { direction = STILL })
	beat(COAL, "recover", settings.coal_recover_ms)
end

behaviours.register("iron_pilgrim_run", {
	{ name = "rest_x", lowest = 0, highest = 1920 },
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "arrive_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "volley_signal_ms", lowest = 1, highest = 60000 },
	{ name = "volley_strike_ms", lowest = 1, highest = 60000 },
	{ name = "volley_recover_ms", lowest = 1, highest = 60000 },
	{ name = "cinders", lowest = 1, highest = 16 },
	{ name = "enraged_cinders", lowest = 1, highest = 16 },
	{ name = "charge_signal_ms", lowest = 1, highest = 60000 },
	{ name = "charge_away_ms", lowest = 1, highest = 60000 },
	{ name = "charge_recover_ms", lowest = 1, highest = 60000 },
	{ name = "brood_signal_ms", lowest = 1, highest = 60000 },
	{ name = "brood_strike_ms", lowest = 1, highest = 60000 },
	{ name = "brood_recover_ms", lowest = 1, highest = 60000 },
	{ name = "brood", lowest = 1, highest = 8 },
	{ name = "coal_signal_ms", lowest = 1, highest = 60000 },
	{ name = "coal_strike_ms", lowest = 1, highest = 60000 },
	{ name = "coal_recover_ms", lowest = 1, highest = 60000 },
	{ name = "coals", lowest = 1, highest = 8 },
	{ name = "enraged_coals", lowest = 1, highest = 8 },
}, function(e)
	local settings = e.settings
	local charge_speed = e.top_speed
	local rest = { x = settings.rest_x, y = rest_row(e, settings) }
	local distance = math.max(1, e.position.x - rest.x)
	behaviours.wait(settings.arrive_ms, {
		direction = LEFT,
		top_speed = distance / (settings.arrive_ms / 1000),
		jumps_to = { x = e.position.x, y = rest.y },
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	e = behaviours.wait(1, { direction = STILL, jumps_to = rest, ends_action = true, top_speed = charge_speed })

	local raged = false
	local next_attack = 1
	while true do
		if e.enraged and not raged then
			raged = true
			beat(RAGE, "enraged", settings.enraged_rest_ms)
		end
		local attack = TURN[next_attack]
		next_attack = next_attack % #TURN + 1
		if attack == VOLLEY then
			volley(settings, raged)
		elseif attack == CHARGE then
			charge(e, settings, rest)
		elseif attack == COAL then
			coal(settings, raged)
		else
			brood(settings)
		end
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)

-- A cinder leaves up and left along the angle its volley gave it, and falls
-- faster the older it is, as fast as it rose after fall_ms.
behaviours.register("cinder_arc", {
	{ name = "fall_ms", lowest = 1, highest = 60000 },
}, function(e)
	local data = e.data
	if data.angle == nil then
		data.angle = math.rad(table.remove(volley_angles, 1) or FIVE_ANGLES[1])
	end
	local rise = math.sin(data.angle)
	local fall = -rise + 2 * rise * e.age_ms / e.settings.fall_ms
	return { direction = { x = -math.cos(data.angle), y = math.clamp(fall, -1, 1) } }
end)

-- A lump of coal flies to where its throw aimed it in flight_ms, along a line
-- bent bump units up at its middle, then lights a fire on the floor there and
-- is gone. As the eggs of the Ash Mother (lobbed_egg), it heads each tick for
-- its place at its age.
behaviours.register("lobbed_coal", {
	{ name = "flight_ms", lowest = 1, highest = 60000 },
	{ name = "bump", lowest = 0, highest = 1080 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.target == nil then
		if #coal_targets == 0 then
			return { direction = LEFT }
		end
		data.target = table.remove(coal_targets, 1)
	end
	local seconds = settings.flight_ms / 1000
	if data.across == nil then
		data.across = (data.target.x - e.position.x) / seconds
		data.down = (data.target.y - e.position.y) / seconds
		data.since_ms = e.age_ms
	end
	local share = (e.age_ms - data.since_ms) / settings.flight_ms
	if share >= 1 then
		behaviours.spawn("pilgrim_coal_fire", { x = data.target.x, y = data.target.y - FIRE_ABOVE })
		behaviours.destroy(e.id)
		return { direction = STILL }
	end
	local bend = -4 * settings.bump * (1 - 2 * share) / seconds
	local up = data.down + bend
	local top = math.max(math.abs(data.across), math.abs(up), 1)
	return { direction = { x = data.across / top, y = up / top }, top_speed = top }
end)
