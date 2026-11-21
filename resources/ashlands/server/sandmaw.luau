--!strict
-- The Sandmaw, the worm that lives under the highway and closes Highway Zero,
-- and what its maw moves: the clods it inhales (sand_debris) and the boulders
-- it spits (sandmaw_boulder). Its sheet is entities/sandmaw.toml.
--
-- It waits below the screen through its warning, then appears where it
-- rests by the right edge and comes up out of its dune (its arrival, whose
-- frames are cut at the top of the dune), and stays there, its maw at floor
-- level facing the ships. Then it opens
-- its maw (the signal of attack 0) and either inhales, a stream of clods
-- flying from the left edge into its maw, which the ships dodge or shoot, or
-- spits boulders along an arc, and a Dunecrawler every second spit; then it
-- closes its maw (the recover). Both use attack 0, the only one its sheet
-- draws. A missile hurts it anywhere on its body once it has come up, while
-- it attacks too; every missile is stopped while it comes up. Once enraged it rests less,
-- inhales a thicker stream, spits more, and picks either at random.

local STILL = { x = 0, y = 0 }

local MAW_ATTACK, RAGE, ARRIVAL = 0, 1, 2

-- Its maw, from the center of its body, in logical units (a pixel of the
-- sheet is 4 units).
local MAW = { x = -110, y = -12 }

-- Where a clod it inhales comes from: the left edge, on a row of the band.
local DEBRIS_COLUMN = 40
local TOP_ROW, BOTTOM_ROW = 220, 820

-- How far in front of the maw a spat body appears, and the row of the floor
-- a spat Dunecrawler walks on.
local SPIT_GAP = 60
local CRAWL_ROW = 848

-- How far below its center the bottom of its dune lies (half its 152 units),
-- less the 8 units it sinks into the floor, so the dune sits on the ground the
-- client draws under its column.
local DUNE_ABOVE_THE_FLOOR = 68

-- The row it rests on: its dune on the floor under its column, or rest_y where
-- the biome gives no ground.
local function rest_row(e, settings): number
	local found, floor = pcall(level.ground, e.position.x)
	if not found or floor == nil then
		return settings.rest_y
	end
	return floor - DUNE_ABOVE_THE_FLOOR
end

-- Where its maw is once it rests, which the clods fly to; a clod is swallowed
-- SWALLOW_GAP units short of it.
local maw_point: { x: number, y: number }? = nil
local SWALLOW_GAP = 40

local function inhale(settings, raged: boolean)
	local every = if raged then settings.enraged_debris_ms else settings.debris_ms
	local left = settings.inhale_ms
	while left > 0 do
		behaviours.spawn("sand_debris", { x = DEBRIS_COLUMN, y = math.random(TOP_ROW, BOTTOM_ROW) })
		behaviours.wait(every, { direction = STILL })
		left -= every
	end
end

local function spit(settings, raged: boolean, with_crawler: boolean)
	local maw = maw_point or { x = 0, y = 0 }
	local from = { x = maw.x - SPIT_GAP, y = maw.y }
	for _ = 1, if raged then settings.enraged_boulders else settings.boulders do
		behaviours.spawn("sandmaw_boulder", from)
		behaviours.wait(settings.boulder_gap_ms, { direction = STILL })
	end
	if with_crawler then
		behaviours.spawn("dunecrawler", { x = from.x, y = CRAWL_ROW })
	end
end

behaviours.register("sandmaw_maw", {
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "open_ms", lowest = 1, highest = 60000 },
	{ name = "inhale_ms", lowest = 1, highest = 60000 },
	{ name = "debris_ms", lowest = 1, highest = 60000 },
	{ name = "enraged_debris_ms", lowest = 1, highest = 60000 },
	{ name = "boulders", lowest = 1, highest = 16 },
	{ name = "enraged_boulders", lowest = 1, highest = 16 },
	{ name = "boulder_gap_ms", lowest = 1, highest = 60000 },
	{ name = "close_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	local arriving = true
	behaviours.pre_hit(function(hit)
		if not arriving then
			return true
		end
		behaviours.destroy(hit.missile)
		return false
	end)

	behaviours.wait(settings.rise_ms, {
		direction = STILL,
		jumps_to = { x = e.position.x, y = rest_row(e, settings) },
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	e = behaviours.wait(1, { direction = STILL, ends_action = true })
	maw_point = { x = e.position.x + MAW.x, y = e.position.y + MAW.y }
	arriving = false

	local raged = false
	local spits = 0
	local inhaling = true
	while true do
		if e.enraged and not raged then
			raged = true
			behaviours.wait(settings.close_ms, { direction = STILL, starts_action = { attack = RAGE, phase = "enraged" } })
			e = behaviours.wait(1, { direction = STILL, ends_action = true })
		end
		behaviours.wait(settings.open_ms, { direction = STILL, starts_action = { attack = MAW_ATTACK, phase = "signal" } })
		behaviours.wait(1, { direction = STILL, starts_action = { attack = MAW_ATTACK, phase = "strike" } })
		if inhaling then
			inhale(settings, raged)
		else
			spits += 1
			spit(settings, raged, spits % 2 == 0)
		end
		behaviours.wait(settings.close_ms, { direction = STILL, starts_action = { attack = MAW_ATTACK, phase = "recover" } })
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
		inhaling = if raged then math.random(0, 1) == 0 else not inhaling
	end
end)

-- A clod flies straight at the maw and is swallowed once it reaches it; it
-- flies right while no maw rests.
behaviours.register("swallowed_debris", {}, function(e)
	local maw = maw_point
	if maw == nil then
		return { direction = { x = 1, y = 0 } }
	end
	local x, y = maw.x - e.position.x, maw.y - e.position.y
	local length = math.sqrt(x * x + y * y)
	if length <= SWALLOW_GAP then
		behaviours.destroy(e.id)
		return { direction = STILL }
	end
	return { direction = { x = x / length, y = y / length } }
end)
