--!strict
-- The Necromancer, the boss of the Pit, and the bodies its attacks bring:
-- the rings its hands throw, the bone spikes of its fist, the skeleton arms
-- it raises and the beam of its gaze. Its sheet is entities/necromancer.toml.
--
-- It slides in from the right, then rests in place and plays its four attacks,
-- each in three beats: a signal the players learn, a strike that always leaves
-- a lane free, and a recover. Its only weak point is its mask: a missile that
-- hits it elsewhere, or while the mask is covered, is stopped, and the mask is
-- uncovered while its rings are out, while its fist is planted and once its
-- gaze has burnt out. Below half its health it hides in its hood (attack 4),
-- then pairs its attacks: rings with the fist, skeleton arms under the gaze.
--
-- Every place below is measured on the frames of the boss, from the center of
-- its body, in logical units (a pixel of the sheet is 4 units); every duration
-- is the length of the animation of its sheet.

local LEFT = { x = -1, y = 0 }
local STILL = { x = 0, y = 0 }

local RINGS, FIST, RAISE, GAZE, RAGE, ARRIVAL = 0, 1, 2, 3, 4, 5

-- The beats of each attack: signal, strike and recover, in milliseconds.
local BEATS = {
	[RINGS] = { 830, 1992, 664 },
	[FIST] = { 1328, 913, 1494 },
	[RAISE] = { 1992, 1660, 664 },
	[GAZE] = { 1494, 830, 1660 },
}

-- The phase in which each attack uncovers the mask.
local UNCOVERED_IN = { [RINGS] = "strike", [FIST] = "recover", [GAZE] = "recover" }

-- The order of the first attacks; the next ones are drawn at random.
local FIRST_ORDER = { RINGS, FIST, GAZE, RAISE }

-- The mask, from the center of the body: its center and its half size, with a
-- margin for the box of a missile.
local MASK = { x = 10, y = -186, half_width = 100, half_height = 76 }

-- Where the hands throw the rings from, and the rows the rings fly along: the
-- top lane, the floor lane, and the middle lane they come back through.
local RING_START = { x = -494, y = 58 }
local TOP_LANE, FLOOR_LANE = -246, 274
local RING_DELAY_MS = 500

-- Where the bone spikes erupt, on each side of the impact of the fist, and
-- the row of the floor they stand on; they erupt as the fist lands,
-- SPIKE_DELAY_MS into its strike.
local SPIKES = { -1034, -906, -586, -458 }
local SPIKE_ROW = 352
local SPIKE_DELAY_MS = 250

-- The fissures the skeleton arms rise from: three of them, one column of the
-- sheet apart, moved together by 0 to 2 steps each time; and their row.
local FISSURE_FIRST, FISSURE_GAP, FISSURE_STEP = -1194, 288, 72
local FISSURE_ROW = 112

-- Where the beam of the gaze lies, its right end at the mask.
local BEAM = { x = -882, y = -206 }

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
	for _, x in SPIKES do
		behaviours.spawn("bone_spike", from(center, { x = x, y = SPIKE_ROW }))
	end
	return SPIKE_DELAY_MS
end

local function spawn_arms(center)
	local shift = math.random(0, 2) * FISSURE_STEP
	for arm = 0, 2 do
		behaviours.spawn("bone_arm", from(center, { x = FISSURE_FIRST + shift + arm * FISSURE_GAP, y = FISSURE_ROW }))
	end
end

behaviours.register("necromancer_rite", {
	{ name = "rest_x", lowest = 0, highest = 1920 },
	{ name = "slide_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "rage_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local uncovered = false
	behaviours.pre_hit(function(hit)
		local on_the_mask = math.abs(hit.offset.x - MASK.x) <= MASK.half_width
			and math.abs(hit.offset.y - MASK.y) <= MASK.half_height
		if uncovered and on_the_mask then
			return true
		end
		behaviours.destroy(hit.missile)
		return false
	end)

	local distance = e.position.x - settings.rest_x
	behaviours.wait(settings.slide_ms, {
		direction = LEFT,
		top_speed = math.max(1, distance * 1000 / settings.slide_ms),
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	e = behaviours.wait(1, {
		direction = STILL,
		jumps_to = { x = settings.rest_x, y = e.position.y },
		ends_action = true,
	})
	local center = { x = settings.rest_x, y = e.position.y }

	local raged = false
	local played = 0
	while true do
		if e.enraged and not raged then
			raged = true
			uncovered = false
			behaviours.wait(settings.rage_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
		end
		played += 1
		local chosen = FIRST_ORDER[played] or math.random(RINGS, GAZE)
		local beats = BEATS[chosen]
		uncovered = false

		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "signal" } })
		if chosen == RAISE or (raged and chosen == GAZE) then
			spawn_arms(center)
		end
		behaviours.wait(beats[1], { direction = STILL })

		uncovered = UNCOVERED_IN[chosen] == "strike"
		behaviours.wait(1, { direction = STILL, starts_action = { attack = chosen, phase = "strike" } })
		local struck_ms = 0
		if chosen == GAZE then
			behaviours.spawn("necromancer_beam", from(center, BEAM))
		end
		if chosen == RINGS or (raged and chosen == FIST) then
			struck_ms += spawn_rings(center)
		end
		if chosen == FIST then
			struck_ms += spawn_spikes(center)
		end
		behaviours.wait(math.max(1, beats[2] - struck_ms), { direction = STILL })

		uncovered = UNCOVERED_IN[chosen] == "recover"
		behaviours.wait(beats[3], { direction = STILL, starts_action = { attack = chosen, phase = "recover" } })
		uncovered = false
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
