--!strict
-- The behaviours of the mobs of Deseret and of what they fire: every mob
-- crosses the playfield from the right to the left; those that attack do it
-- in three beats the client draws from their sheet (signal, strike, recover).
-- Their sheets are in entities/, their sprites in sprites/deseret/. Every
-- attack warns: a white eye, a lit fuse, a pink sight.
--
-- A missile a script steers is steered once when its sheet is read, with no
-- ship and no body behind the handle: its function then reads only its
-- settings and its age.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }
local FULL_TURN = 2 * math.pi

-- The right edge of the playfield: a mob attacks only once it shows.
local RIGHT_EDGE = 1920

-- How long a tick lasts, in seconds, and how fast the ground scrolls left,
-- in logical units per second (LEVEL_SCROLL_SPEED): a walker that stops to
-- attack keeps moving left with the ground under it.
local TICK_SECONDS = 1 / 60
local SCROLL_SPEED = 120

-- How far the center of each walker stands above the floor (negative), or a
-- Lantern below the ceiling (positive): the gaps deseret.luau spawns them
-- with.
local TUMBLECAGE_GAP = -40
local SURVEYOR_GAP = -60
local LANTERN_GAP = 140

-- Where what a mob fires leaves it, from its center: under the basket of an
-- Aeronaut, under the glass of a Lantern, in front of the lens of a
-- Surveyor.
local BASKET_DROP = 90
local GLASS_DROP = 60
local LENS = { x = -60, y = -28 }

-- Half the length of the sight of a Surveyor (1920 units): its center lies
-- that far left of the lens.
local SIGHT_HALF = 960

-- The share of their spread each of the four barbs of a Tumblecage takes,
-- in the order they appear.
local BARB_SHARES = { -1, -1 / 3, 1 / 3, 1 }
local next_barb = 0

local function percent(value: number): number
	return value / 100
end

-- The direction that keeps a body `gap` units from the floor (or the
-- ceiling) of the biome under its column while it moves left at its top
-- speed; straight left where the biome has no ground.
local function along(e, read: (number) -> number?, gap: number)
	local found, row = pcall(read, e.position.x)
	if not found or row == nil then
		return LEFT
	end
	local step = (row + gap - e.position.y) / (e.top_speed * TICK_SECONDS)
	return { x = -1, y = math.clamp(step, -1, 1) }
end

-- The direction of a walker that stands still on the ground: left at the
-- speed of the scroll, so the ground does not slide under it.
local function planted(e)
	return { x = -math.min(1, SCROLL_SPEED / e.top_speed), y = 0 }
end

-- Whether a body is rested: idle, and idle for at least rest_ms.
local function rested(e, rest_ms: number): boolean
	return e.action == nil and e.action_age_ms >= rest_ms
end

-- The direction of a body that flies left, bobbing `bob` percent of its
-- speed around its row every bob_ms.
local function bobbing_left(e)
	local settings = e.settings
	return { x = -1, y = percent(settings.bob) * math.sin(FULL_TURN * e.age_ms / settings.bob_ms) }
end

local function heading_to(from, to)
	local x, y = to.x - from.x, to.y - from.y
	local length = math.sqrt(x * x + y * y)
	if length <= 0 then
		return LEFT, 0
	end
	return { x = x / length, y = y / length }, length
end

-- Plays the three beats of attack 0, the body moving `still` meanwhile: the
-- signal, then the strike, during which `on_strike` creates what the body
-- fires, then the recover.
local function attack(settings, still, on_strike: () -> ())
	behaviours.wait(settings.signal_ms, { direction = still, starts_action = { attack = 0, phase = "signal" } })
	behaviours.wait(settings.strike_ms, { direction = still, starts_action = { attack = 0, phase = "strike" } })
	on_strike()
	behaviours.wait(settings.recover_ms, { direction = still, starts_action = { attack = 0, phase = "recover" } })
	behaviours.wait(1, { direction = still, ends_action = true })
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

-- Whether a Carrion dives at `ship`: ahead of it within `reach` units, and
-- at least `drop` units above or below its row, so the dive is a slant down
-- (or up) at the ship and never a flight along the row its shots fly.
local function dives_at(e, ship): boolean
	local settings = e.settings
	local ahead = e.position.x - ship.x
	return ahead > 0 and ahead <= settings.reach and math.abs(ship.y - e.position.y) >= settings.drop
end

-- The Carrion flies left, bobbing; once a ship is within reach ahead of it
-- and above or below its row, and it is rested, it raises its wings, notes
-- where that ship is, and once the signal is over dives there, at dive_speed
-- for strike_ms at most: where the ship was aim_lag_ms before, the length of
-- its signal. Then it climbs back.
behaviours.register("carrion_dive", with_attack({
	{ name = "bob", lowest = 0, highest = 100 },
	{ name = "bob_ms", lowest = 1, highest = 60000 },
	{ name = "reach", lowest = 0, highest = 1920 },
	{ name = "drop", lowest = 0, highest = 1080 },
	{ name = "aim_lag_ms", lowest = 0, highest = 60000 },
	{ name = "dive_speed", lowest = 1, highest = 4000 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if ship ~= nil and dives_at(e, ship) and rested(e, settings.rest_ms) then
		local cruise = e.top_speed
		local target = { x = ship.x, y = ship.y }
		e = behaviours.wait(settings.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
		local heading, distance = heading_to(e.position, target)
		local dive_ms = math.clamp(distance / settings.dive_speed * 1000, 1, settings.strike_ms)
		behaviours.wait(dive_ms, {
			direction = heading,
			top_speed = settings.dive_speed,
			starts_action = { attack = 0, phase = "strike" },
		})
		behaviours.wait(settings.recover_ms, {
			direction = { x = -0.4, y = -1 },
			top_speed = cruise,
			starts_action = { attack = 0, phase = "recover" },
		})
		behaviours.wait(1, { direction = LEFT, ends_action = true })
	end
	return { direction = bobbing_left(e) }
end)

-- The Shroud drifts left; a ship within `align` units of its row, while it
-- shows and rested, makes it glide to that row as its cloth stretches flat,
-- then sweep left at sweep_speed until it leaves the screen.
behaviours.register("shroud_sweep", {
	{ name = "align", lowest = 0, highest = 1080 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "sweep_speed", lowest = 1, highest = 4000 },
}, function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if
		ship ~= nil
		and e.position.x < RIGHT_EDGE
		and rested(e, settings.rest_ms)
		and math.abs(ship.y - e.position.y) <= settings.align
	then
		local rise = ship.y - e.position.y
		local glide = math.max(1, math.abs(rise) / (settings.signal_ms / 1000))
		behaviours.wait(settings.signal_ms, {
			direction = { x = 0, y = if rise < 0 then -1 else 1 },
			top_speed = glide,
			starts_action = { attack = 0, phase = "signal" },
		})
		behaviours.wait(1, {
			direction = LEFT,
			top_speed = settings.sweep_speed,
			starts_action = { attack = 0, phase = "strike" },
		})
		while true do
			behaviours.wait(1000, { direction = LEFT })
		end
	end
	return { direction = LEFT }
end)

-- The Aeronaut drifts left along the top, bobbing; left of fire_from_x and
-- rested, its fuse burns, then it lets a stick of dynamite fall.
behaviours.register("aeronaut_drift", with_attack({
	{ name = "bob", lowest = 0, highest = 100 },
	{ name = "bob_ms", lowest = 1, highest = 60000 },
	{ name = "fire_from_x", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	if e.position.x <= settings.fire_from_x and rested(e, settings.rest_ms) then
		local basket = { x = e.position.x, y = e.position.y + BASKET_DROP }
		attack(settings, STILL, function()
			behaviours.spawn("aeronaut_stick", basket)
		end)
	end
	return { direction = bobbing_left(e) }
end)

-- A Cricket weaves left around its row, its wave shifted by the row so a
-- swarm ripples. Left of a column its row sets, it lights up, tells the
-- crickets within `reach` (cricket_lit), and heads for the nearest ship with
-- them; a cricket told so heads there at once.
behaviours.register("cricket_swarm", {
	{ name = "amplitude", lowest = 0, highest = 200 },
	{ name = "period_ms", lowest = 1, highest = 60000 },
	{ name = "light_from_x", lowest = 0, highest = 1920 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "reach", lowest = 0, highest = 1920 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.row == nil then
		data.row = e.position.y
		local own = e.id
		bus.on("cricket_lit", function(event)
			if event.id ~= own and data.heading == nil and data.place ~= nil then
				local x, y = event.x - data.place.x, event.y - data.place.y
				if x * x + y * y <= settings.reach * settings.reach then
					data.turn = true
				end
			end
		end)
	end
	data.place = { x = e.position.x, y = e.position.y }
	if data.heading ~= nil then
		return { direction = data.heading }
	end
	local ship = e.nearest_ship
	local lights_at = settings.light_from_x - (data.row * 37) % 500
	if ship ~= nil and e.position.x < RIGHT_EDGE and (data.turn or e.position.x <= lights_at) then
		if not data.turn then
			bus.emit("cricket_lit", { id = e.id, x = e.position.x, y = e.position.y })
			e = behaviours.wait(settings.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
		end
		data.heading = heading_to(e.position, ship)
		behaviours.wait(1, { direction = data.heading, starts_action = { attack = 0, phase = "strike" } })
		return { direction = data.heading }
	end
	local phase = data.row / 120
	local target = data.row + settings.amplitude * math.sin(FULL_TURN * e.age_ms / settings.period_ms + phase)
	local step = (target - e.position.y) / (e.top_speed * TICK_SECONDS)
	return { direction = { x = -1, y = math.clamp(step, -1, 1) } }
end)

-- The dust devil drifts left, swaying; it swallows every missile that
-- crosses it, shows its lighter frame for its last warn_ms, and is gone after
-- life_ms.
behaviours.register("dust_drift", {
	{ name = "sway", lowest = 0, highest = 1080 },
	{ name = "sway_ms", lowest = 1, highest = 60000 },
	{ name = "life_ms", lowest = 1, highest = 60000 },
	{ name = "warn_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.row == nil then
		data.row = e.position.y
		behaviours.pre_hit(function(hit)
			behaviours.destroy(hit.missile)
			return false
		end)
	end
	if e.age_ms >= settings.life_ms then
		behaviours.destroy(e.id)
		return { direction = STILL }
	end
	if e.action == nil and e.age_ms >= settings.life_ms - settings.warn_ms then
		return { direction = LEFT, starts_action = { attack = 0, phase = "signal" } }
	end
	local target = data.row + settings.sway * math.sin(FULL_TURN * e.age_ms / settings.sway_ms)
	local step = (target - e.position.y) / (e.top_speed * TICK_SECONDS)
	return { direction = { x = -1, y = math.clamp(step, -1, 1) } }
end)

-- The Lantern floats left under the ceiling, keeping to it; a ship below it
-- within reach of its column whitens its glass, and it drops a bead of oil.
behaviours.register("lantern_float", with_attack({
	{ name = "reach", lowest = 0, highest = 1920 },
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
		local glass = { x = e.position.x, y = e.position.y + GLASS_DROP }
		attack(settings, LEFT, function()
			behaviours.spawn("lantern_drop", glass)
		end)
	end
	return { direction = along(e, level.ceiling, LANTERN_GAP) }
end)

-- Where each Tumblecage rolls, by network id, for its hit handler: it writes
-- it each tick, and forgets it as it rolls past the left edge.
local rolling: { [number]: { x: number, y: number } } = {}

-- The Tumblecage rolls left along the floor, keeping to it, up and down the
-- steps of the ground.
behaviours.register("tumblecage_roll", {}, function(e)
	if e.position.x < 0 then
		rolling[e.id] = nil
	else
		rolling[e.id] = { x = e.position.x, y = e.position.y }
	end
	return { direction = along(e, level.ground, TUMBLECAGE_GAP) }
end)

-- A Tumblecage shot down throws four barbs up, spread on both sides.
behaviours.on_hit("tumblecage_break", function(hit)
	if not hit.destroyed then
		return
	end
	local place = rolling[hit.id]
	rolling[hit.id] = nil
	if place == nil then
		return
	end
	for _ = 1, #BARB_SHARES do
		behaviours.spawn("tumblecage_barb", place)
	end
end)

-- The direction of a missile that falls on the ground, which scrolls
-- left under it: the ground carries it left as fast as the scroll.
local function falling_with_the_ground(e)
	return { x = -math.min(1, SCROLL_SPEED / e.top_speed), y = 1 }
end

-- A barb rises, drifting to its side of the spread around the ground that
-- scrolls under it, then falls back.
behaviours.register("barb_toss", {
	{ name = "spread", lowest = 0, highest = 100 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.share == nil then
		next_barb = next_barb % #BARB_SHARES + 1
		data.share = BARB_SHARES[next_barb]
	end
	local climb = -1 + 2 * e.age_ms / settings.rise_ms
	local drift = data.share * percent(settings.spread) - SCROLL_SPEED / e.top_speed
	return { direction = { x = drift, y = math.clamp(climb, -1, 1) } }
end)

-- The Surveyor walks left on its tripod along the floor; left of
-- fire_from_x and rested, it plants itself, locks its sight on the row of the
-- nearest ship, then fires once along it.
behaviours.register("surveyor_aim", with_attack({
	{ name = "fire_from_x", lowest = 0, highest = 1920 },
}), function(e)
	local settings = e.settings
	local ship = e.nearest_ship
	if ship ~= nil and e.position.x <= settings.fire_from_x and rested(e, settings.rest_ms) then
		local row = ship.y
		local lens = { x = e.position.x + LENS.x, y = row }
		behaviours.spawn("surveyor_sight", { x = lens.x - SIGHT_HALF, y = row })
		e = behaviours.wait(settings.signal_ms, { direction = planted(e), starts_action = { attack = 0, phase = "signal" } })
		behaviours.spawn("surveyor_shot", { x = e.position.x + LENS.x, y = row })
		behaviours.wait(settings.strike_ms, { direction = planted(e), starts_action = { attack = 0, phase = "strike" } })
		behaviours.wait(settings.recover_ms, { direction = planted(e), starts_action = { attack = 0, phase = "recover" } })
		behaviours.wait(1, { direction = planted(e), ends_action = true })
	end
	return { direction = along(e, level.ground, SURVEYOR_GAP) }
end)

-- The sight of a Surveyor stays on its row for life_ms, then is gone.
behaviours.register("sight_hold", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	if e.age_ms >= e.settings.life_ms then
		behaviours.destroy(e.id)
	end
	return { direction = STILL }
end)

-- A missile that flies straight left.
behaviours.register("flying_left", {}, function(_e)
	return { direction = LEFT }
end)

-- A missile that falls straight down on the ground, which scrolls under it.
behaviours.register("falling_straight", {}, function(e)
	return { direction = falling_with_the_ground(e) }
end)

-- The row of the ground under a body, or floor_y where the biome gives none.
local function ground_under(e, floor_y: number): number
	local found, row = pcall(level.ground, e.position.x)
	if not found or row == nil then
		return floor_y
	end
	return row
end

-- A stick of dynamite falls drifting left; blast_above units over the
-- ground it blows up: the blast of a crate on itself, which kills it, so it
-- dies in its burst, and hurts the ships around.
behaviours.register("dynamite_fall", {
	{ name = "drift", lowest = 0, highest = 100 },
	{ name = "blast_above", lowest = 0, highest = 1080 },
	{ name = "floor_y", lowest = 0, highest = 1080 },
}, function(e)
	local settings = e.settings
	if e.data.burst then
		return { direction = STILL }
	end
	if e.position.y >= ground_under(e, settings.floor_y) - settings.blast_above then
		e.data.burst = true
		behaviours.spawn("dynamite_blast", e.position)
		behaviours.spawn("dynamite_heat", e.position)
		return { direction = STILL }
	end
	return { direction = { x = -percent(settings.drift), y = 1 } }
end)

-- A bead of oil falls, and lights a fire on the ground under it.
behaviours.register("oil_fall", {
	{ name = "floor_y", lowest = 0, highest = 1080 },
}, function(e)
	local ground = ground_under(e, e.settings.floor_y)
	if e.position.y >= ground then
		behaviours.spawn("ground_fire", { x = e.position.x, y = ground - 64 })
		behaviours.destroy(e.id)
		return { direction = STILL }
	end
	return { direction = falling_with_the_ground(e) }
end)
