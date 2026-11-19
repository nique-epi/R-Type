--!strict
-- The Necromancer, the boss of the Pit, and the bodies its attacks bring:
-- the rings its hands throw, the bone spikes of its fist, the skeleton arms
-- it raises and the beam of its gaze. Its sheet is entities/necromancer.toml.
--
-- It rises out of the floor where it stands, then plays in place its four
-- attacks, each in three beats: a signal the players learn, a strike that
-- always leaves a lane free, and a recover. A missile that touches its hood or
-- its cape hurts it; every missile is stopped while it comes up or hides in its
-- hood. Below half its health it hides in its hood (attack 4), then pairs
-- its attacks: rings with the fist, skeleton arms under the gaze.
--
-- Every place below is measured from the center of its body, in logical units
-- (a pixel of the sheet is 4 units), the boss resting at {1544, 598}: what
-- comes out of its body (its hands, its fist, its mask) is placed on its
-- frames, and what reaches across the Pit (the lanes of the rings, the
-- fissures, the spikes, the beam) on the playfield, so its attacks cover the
-- whole width the ships fly in. Every duration is the length of the animation
-- of its sheet.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }

local RINGS, FIST, RAISE, GAZE, RAGE, ARRIVAL = 0, 1, 2, 3, 4, 5

-- The beats of each attack: signal, strike and recover, in milliseconds.
local BEATS = {
	[RINGS] = { 830, 1992, 664 },
	[FIST] = { 1328, 913, 1411 },
	[RAISE] = { 1909, 1577, 664 },
	[GAZE] = { 1494, 830, 1660 },
}

-- The order of the first attacks; the next ones are drawn at random.
local FIRST_ORDER = { RINGS, FIST, GAZE, RAISE }

-- Where the hands throw the rings from, and the rows the rings fly along: the
-- top lane, row 300, and the floor lane, row 760; they come back through the
-- middle lane of their sheet, row 530.
local RING_START = { x = -296, y = 35 }
local TOP_LANE, FLOOR_LANE = -298, 162
local RING_DELAY_MS = 500

-- The bone spikes erupt as the fist lands, SPIKE_DELAY_MS into its strike,
-- then one after the other, SPIKE_STEP_MS apart, along the floor towards the
-- left edge: SPIKE_COUNT of them, the first SPIKE_FIRST from the center of the
-- body (x 976, left of the impact of the fist at x 1096), SPIKE_GAP apart. A
-- spike stands on the floor of the Pit (row 848): its center is SPIKE_ROW
-- below the center of the body (row 782).
local SPIKE_FIRST, SPIKE_GAP, SPIKE_COUNT = -568, 120, 8
local SPIKE_ROW = 184
local SPIKE_DELAY_MS = 250
local SPIKE_STEP_MS = 90

-- The fissures the skeleton arms rise from: three of them across the left of
-- the floor, from x 160, 288 units apart, moved together by 0 to 2 steps of 72
-- units each time; and their row.
local FISSURE_FIRST, FISSURE_GAP, FISSURE_STEP = -1384, 288, 72
local FISSURE_ROW = 98

-- Where the beam of the gaze lies: its 1600 units end at the mask, on the
-- row of its eyes, so it crosses the Pit to the left edge. It comes with the
-- signal of the gaze and shows its aim until the strike (necromancer_gaze).
local BEAM = { x = -825, y = -110 }

-- How long a body at `speed` units per second takes to cross `distance`
-- units, in milliseconds: one tick at least.
local function crossing_ms(distance: number, speed: number): number
	return math.max(1, math.abs(distance) * 1000 / speed)
end

local function from(center, place)
	return { x = center.x + place.x, y = center.y + place.y }
end

-- Each returns the milliseconds of the strike it waited, so the strike lasts
-- the length of its animation whatever it spawns.
local function spawn_rings(center): number
	behaviours.spawn("necromancer_ring", from(center, { x = RING_START.x, y = TOP_LANE }))
	behaviours.wait(RING_DELAY_MS, { direction = STILL })
	behaviours.spawn("necromancer_ring", from(center, { x = RING_START.x, y = FLOOR_LANE }))
	return RING_DELAY_MS
end

local function spawn_spikes(center): number
	behaviours.wait(SPIKE_DELAY_MS, { direction = STILL })
	for spike = 0, SPIKE_COUNT - 1 do
		behaviours.spawn("bone_spike", from(center, { x = SPIKE_FIRST - spike * SPIKE_GAP, y = SPIKE_ROW }))
		if spike < SPIKE_COUNT - 1 then
			behaviours.wait(SPIKE_STEP_MS, { direction = STILL })
		end
	end
	return SPIKE_DELAY_MS + (SPIKE_COUNT - 1) * SPIKE_STEP_MS
end

local function spawn_arms(center)
	local shift = math.random(0, 2) * FISSURE_STEP
	for arm = 0, 2 do
		behaviours.spawn("bone_arm", from(center, { x = FISSURE_FIRST + shift + arm * FISSURE_GAP, y = FISSURE_ROW }))
	end
end

behaviours.register("necromancer_rite", {
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local uncovered = false
	behaviours.pre_hit(function(hit)
		if uncovered then
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
	e = behaviours.wait(1, { direction = STILL, ends_action = true })
	local center = { x = e.position.x, y = settings.rest_y }
	uncovered = true

	local raged = false
	local played = 0
	while true do
		if e.enraged and not raged then
			raged = true
			uncovered = false
			behaviours.wait(settings.rage_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
			uncovered = true
		end
		played += 1
		local chosen = FIRST_ORDER[played] or math.random(RINGS, GAZE)
		local beats = BEATS[chosen]

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "signal" } })
		if chosen == RAISE or (raged and chosen == GAZE) then
			spawn_arms(center)
		end
		if chosen == GAZE then
			behaviours.spawn("necromancer_beam", from(center, BEAM))
		end
		behaviours.wait(beats[1], { direction = STILL })

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		local struck_ms = 0
		if chosen == RINGS or (raged and chosen == FIST) then
			struck_ms += spawn_rings(center)
		end
		if chosen == FIST then
			struck_ms += spawn_spikes(center)
		end
		behaviours.wait(math.max(1, beats[2] - struck_ms), { direction = STILL })

		behaviours.wait(beats[3], { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)

-- A ring thrown by a hand: it flies left along its lane to turn_x, sweeps to
-- the middle lane, flies back right along it, and is removed at return_x.
behaviours.register("necromancer_ring", {
	{ name = "turn_x", lowest = 0, highest = 1920 },
	{ name = "middle_y", lowest = 0, highest = 1080 },
	{ name = "return_x", lowest = 0, highest = 1920 },
}, function(e)
	local settings = e.settings
	local speed = e.top_speed
	local sweep = settings.middle_y - e.position.y
	behaviours.wait(crossing_ms(e.position.x - settings.turn_x, speed), { direction = LEFT })
	behaviours.wait(crossing_ms(sweep, speed), { direction = { x = 0, y = if sweep > 0 then 1 else -1 } })
	behaviours.wait(crossing_ms(settings.return_x - settings.turn_x, speed), { direction = { x = 1, y = 0 } })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- The beam of the gaze: it shows its aim, a blinking line where it will
-- burn, for aim_ms, the signal of the gaze, then burns for burn_ms, the strike
-- in which the hurt zone of the Necromancer burns that line, and is removed.
behaviours.register("necromancer_gaze", {
	{ name = "aim_ms", lowest = 1, highest = 60000 },
	{ name = "burn_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	behaviours.wait(settings.aim_ms, { direction = STILL, look = "aim" })
	behaviours.wait(settings.burn_ms, { direction = STILL, look = "" })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- A body an attack of the Necromancer brings that stays in place: a bone
-- spike, a skeleton arm or the beam of its gaze. It shows its idle loop for
-- strike_after_ms, plays its strike (attack 0) until life_ms, then is removed.
behaviours.register("necromancer_effect", {
	{ name = "strike_after_ms", lowest = 0, highest = 60000 },
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	if settings.strike_after_ms > 0 then
		behaviours.wait(settings.strike_after_ms, { direction = STILL })
	end
	if settings.strike_after_ms < settings.life_ms then
		behaviours.wait(settings.life_ms - settings.strike_after_ms, {
			direction = STILL,
			starts_action = { attack = 0, phase = "strike" },
		})
	end
	behaviours.destroy(e.id)
	return { direction = STILL }
end)
