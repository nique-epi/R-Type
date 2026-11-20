--!strict
-- The decor of the Ashlands that does something, laid on top of the maps of
-- its biomes, and the effects the bosses leave in place. The maps are only a
-- picture: nothing painted on them can hurt, break or move, so each of these
-- is a body the level creates past the right edge, which the scroll then
-- carries (`motion.kind = "scroll"` in its sheet).
--
-- A hazard hurts through the hurt zone of its sheet only, during the strike
-- of its cycle; a breakable is an enemy a missile destroys, and its hit
-- handler bursts it; a trigger is an obstacle nothing touches, which calls
-- mobs once. Every hazard follows the boss on screen: it is removed once that
-- boss is killed, so a fight whose scroll is paused ends with the boss.

local STILL = { x = 0, y = 0 }
local RIGHT = { x = 1, y = 0 }

-- Past the right edge of the playfield, where a mob a trigger calls enters,
-- and the right edge itself, which a trigger acts left of.
local ENTRY_COLUMN = 1984
local RIGHT_EDGE = 1920

-- The rows the Scrapdrones a searchlight calls fly along, and the row the
-- Fixbots an alarm calls roll along.
local DRONE_ROWS = { 300, 520, 740 }
local FIXBOT_ROW = 840
local FIXBOT_GAP_MS = 400

-- The floor of the highway (row 896): where a puddle and a fallen billboard lie.
local PUDDLE_ROW = 884
local WRECK_ROW = 836

-- How often a body that watches the bus or waits for a column looks again,
-- in milliseconds.
local STEP_MS = 100

-- Where a blast of a barrel lands around it: on it, and on each side.
local BLAST_PLACES = { { x = 0, y = 0 }, { x = -110, y = 0 }, { x = 110, y = 0 }, { x = 0, y = -110 } }

-- How far left of a billboard its wreck lies: it falls to the left.
local WRECK_SHIFT = -120

-- How far below a leak its drop falls from, and from where a grille lets a
-- Cindermite out.
local DRIP_DROP = 70
local GRILLE_MOUTH = -70

-- The angles a searchlight sweeps, in degrees above the left, in the order of
-- its looks, and how far off its beam a ship is still caught; where its lamp
-- lies from the center of its body.
local BEAM_ANGLES = { 20, 40, 60, 80, 100 }
local CAUGHT_DEGREES = 12
local LAMP = { x = 192, y = 92 }

-- Where a breakable stands, by network id: a hit handler is told the id of
-- the body it serves, not where it is, and a breakable writes it each tick
-- while it shows, and forgets it as it scrolls past the left edge.
local standing: { [number]: { x: number, y: number } } = {}

-- A hazard learns which boss is on screen from its health, and is removed
-- with that boss.
local function follow_the_boss(e)
	local own = e.id
	local data = e.data
	bus.on("boss_health_changed", function(event)
		data.boss = event.id
	end)
	bus.on("entity_destroyed", function(event)
		if data.boss ~= nil and event.id == data.boss then
			behaviours.destroy(own)
		end
	end)
end

local CYCLE_SETTINGS = {
	{ name = "first_ms", lowest = 1, highest = 60000 },
	{ name = "idle_ms", lowest = 1, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "strike_ms", lowest = 1, highest = 60000 },
	{ name = "recover_ms", lowest = 1, highest = 60000 },
}

-- One cycle of a hazard: it signals, strikes for strike_ms, recovers, and is
-- idle again.
local function erupt(settings, strike_ms: number)
	behaviours.wait(settings.signal_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	behaviours.wait(strike_ms, { direction = STILL, starts_action = { attack = 0, phase = "strike" } })
	behaviours.wait(settings.recover_ms, { direction = STILL, starts_action = { attack = 0, phase = "recover" } })
	behaviours.wait(1, { direction = STILL, ends_action = true })
end

-- A geyser, a live cable or control rods: idle first_ms, then cycles forever.
behaviours.register("hazard_cycle", CYCLE_SETTINGS, function(e)
	local settings = e.settings
	follow_the_boss(e)
	behaviours.wait(settings.first_ms, { direction = STILL })
	while true do
		erupt(settings, settings.strike_ms)
		behaviours.wait(settings.idle_ms, { direction = STILL })
	end
end)

-- A steam vent cycles like a hazard, and jets at once, as long as the purge
-- lasts, when the Glowheart purges.
behaviours.register("steam_vent_cycle", CYCLE_SETTINGS, function(e)
	local settings = e.settings
	local data = e.data
	follow_the_boss(e)
	bus.on("glowheart_purge", function(event)
		data.purge_ms = event.ms
	end)
	local idle_left = settings.first_ms
	while true do
		behaviours.wait(STEP_MS, { direction = STILL })
		idle_left -= STEP_MS
		if data.purge_ms ~= nil then
			local purge = data.purge_ms
			data.purge_ms = nil
			erupt(settings, purge)
			idle_left = settings.idle_ms
		elseif idle_left <= 0 then
			erupt(settings, settings.strike_ms)
			idle_left = settings.idle_ms
		end
	end
end)

-- A puddle a barrel leaves: it glows life_ms, then is gone.
behaviours.register("fading_hazard", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	follow_the_boss(e)
	behaviours.wait(e.settings.life_ms, { direction = STILL })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- A blast or a wreck, a missile of no player that hurts the mobs it touches:
-- it lives life_ms. It heads right, the edge that removes a missile of a
-- player, at the one unit per second of its sheet.
behaviours.register("blast_life", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	if e.age_ms >= e.settings.life_ms then
		behaviours.destroy(e.id)
	end
	return { direction = RIGHT }
end)

-- A barrel or a billboard: it writes where it stands for its hit handler.
behaviours.register("breakable_watch", {}, function(e)
	if e.position.x < 0 then
		standing[e.id] = nil
		return { direction = STILL }
	end
	local place = standing[e.id]
	if place == nil then
		standing[e.id] = { x = e.position.x, y = e.position.y }
	else
		place.x, place.y = e.position.x, e.position.y
	end
	return { direction = STILL }
end)

local function burst_from(id: number): { x: number, y: number }?
	local place = standing[id]
	standing[id] = nil
	return place
end

-- A barrel shot down blows up around it and leaves a glowing puddle.
behaviours.on_hit("barrel_burst", function(hit)
	if not hit.destroyed then
		return
	end
	local place = burst_from(hit.id)
	if place == nil then
		return
	end
	for _, around in BLAST_PLACES do
		behaviours.spawn("toxic_blast", { x = place.x + around.x, y = place.y + around.y })
	end
	behaviours.spawn("toxic_puddle", { x = place.x, y = PUDDLE_ROW })
end)

-- A billboard shot down falls to the left across the floor.
behaviours.on_hit("billboard_fall", function(hit)
	if not hit.destroyed then
		return
	end
	local place = burst_from(hit.id)
	if place ~= nil then
		behaviours.spawn("billboard_wreck", { x = place.x + WRECK_SHIFT, y = WRECK_ROW })
	end
end)

-- The degrees above the left at which a ship lies from a lamp.
local function bearing(lamp, ship): number
	return math.deg(math.atan2(lamp.y - ship.y, lamp.x - ship.x))
end

-- A searchlight sweeps its beam across the sky, one look per angle, there and
-- back over sweep_ms; the first ship caught in it calls `drones` Scrapdrones,
-- once.
behaviours.register("searchlight_sweep", {
	{ name = "sweep_ms", lowest = 1, highest = 60000 },
	{ name = "reach", lowest = 0, highest = 4096 },
	{ name = "drones", lowest = 1, highest = 3 },
}, function(e)
	local settings = e.settings
	local step_ms = settings.sweep_ms / (2 * #BEAM_ANGLES - 2)
	local called = false
	local index, step = 1, 1
	while true do
		local angle = BEAM_ANGLES[index]
		e = behaviours.wait(step_ms, { direction = STILL, look = "beam_" .. angle })
		local ship = e.nearest_ship
		if not called and ship ~= nil and e.position.x < RIGHT_EDGE then
			local lamp = { x = e.position.x + LAMP.x, y = e.position.y + LAMP.y }
			local x, y = ship.x - lamp.x, ship.y - lamp.y
			if x * x + y * y <= settings.reach * settings.reach and math.abs(bearing(lamp, ship) - angle) <= CAUGHT_DEGREES then
				called = true
				for drone = 1, settings.drones do
					behaviours.spawn("scrapdrone", { x = ENTRY_COLUMN, y = DRONE_ROWS[drone] })
				end
			end
		end
		if index + step < 1 or index + step > #BEAM_ANGLES then
			step = -step
		end
		index += step
	end
end)

-- An alarm beacon spins once it passes wake_x and sends `fixbots` Fixbots
-- rolling in from the right, one after the other.
behaviours.register("alarm_wake", {
	{ name = "wake_x", lowest = 0, highest = 1920 },
	{ name = "fixbots", lowest = 1, highest = 8 },
}, function(e)
	local settings = e.settings
	while e.position.x > settings.wake_x do
		e = behaviours.wait(STEP_MS, { direction = STILL })
	end
	for fixbot = 1, settings.fixbots do
		behaviours.spawn("fixbot", { x = ENTRY_COLUMN, y = FIXBOT_ROW })
		if fixbot < settings.fixbots then
			behaviours.wait(FIXBOT_GAP_MS, { direction = STILL, look = "spinning" })
		end
	end
	while true do
		behaviours.wait(STEP_MS * 10, { direction = STILL, look = "spinning" })
	end
end)

-- A vent grille lets a Cindermite out every interval_ms once left of
-- spawn_from_x, until a missile shuts it.
behaviours.register("vent_spawner", {
	{ name = "interval_ms", lowest = 1, highest = 60000 },
	{ name = "spawn_from_x", lowest = 0, highest = 1920 },
}, function(e)
	local settings = e.settings
	if e.position.x > settings.spawn_from_x then
		behaviours.wait(STEP_MS, { direction = STILL })
		return { direction = STILL }
	end
	behaviours.spawn("cindermite", { x = e.position.x + GRILLE_MOUTH, y = e.position.y })
	behaviours.wait(settings.interval_ms, { direction = STILL })
	return { direction = STILL }
end)

-- Shows `look` for as long as the last event heard asked, then its idle loop.
local function lit_by(event_name: string, look: string, e, on_step: ((number) -> ())?)
	local data = e.data
	data.lit_ms = 0
	bus.on(event_name, function(event)
		data.lit_ms = event.ms
	end)
	while true do
		e = behaviours.wait(STEP_MS, { direction = STILL, look = if data.lit_ms > 0 then look else "" })
		data.lit_ms = math.max(0, data.lit_ms - STEP_MS)
		if on_step ~= nil then
			on_step(e.position.x)
		end
	end
end

-- A coolant leak lets a drop fall every drip_ms, and lights up with the
-- signal of the Fallout rain.
behaviours.register("coolant_drip", {
	{ name = "drip_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local row = e.position.y + DRIP_DROP
	local since = 0
	lit_by("glowheart_rain", "lit", e, function(x: number)
		since += STEP_MS
		if since >= settings.drip_ms and x < RIGHT_EDGE then
			since = 0
			behaviours.spawn("glowheart_drop", { x = x, y = row })
		end
	end)
	return { direction = STILL }
end)

-- A meltdown lamp flashes with the signal of the Meltdown.
behaviours.register("meltdown_lamp_watch", {}, function(e)
	lit_by("glowheart_meltdown", "flashing", e, nil)
	return { direction = STILL }
end)

-- A body an attack of a boss leaves in place: a tendril or the beam of a
-- breath. It shows its aim (its look `aim`) for strike_after_ms, then strikes
-- (attack 0, which its hurt zone names) until life_ms, then is removed.
behaviours.register("lingering_effect", {
	{ name = "strike_after_ms", lowest = 1, highest = 60000 },
	{ name = "life_ms", lowest = 2, highest = 60000 },
}, function(e)
	local settings = e.settings
	behaviours.wait(settings.strike_after_ms, { direction = STILL, look = "aim" })
	behaviours.wait(math.max(1, settings.life_ms - settings.strike_after_ms), {
		direction = STILL,
		look = "",
		starts_action = { attack = 0, phase = "strike" },
	})
	behaviours.destroy(e.id)
	return { direction = STILL }
end)
