--!strict
-- The Old Widow, the windpump of an abandoned farm at the top of the mesa,
-- and what she throws: the gusts of her gale (widow_gale), her blades
-- (widow_blade), the cracks and the geysers of her pump (widow_crack,
-- widow_geyser) and the tatters of her veil (widow_tatter). Her sheet is
-- entities/old_widow.toml.
--
-- She waits below the screen through her warning, then rises out of the
-- mesa where she rests (her arrival, attack 4, whose frames are cut at the
-- top of her butte) and her wheel starts. Then she takes her attacks in turn,
-- calm between two: the gale (attack 0), sand blown along two lanes of
-- five; the blades (attack 1), two blades thrown to the left edge and back;
-- the pump (attack 2), the ground cracking under the nearest ship and at two
-- other places, then a geyser of oil out of each crack. Once enraged she
-- rests less and tears her veil (attack 3) every third attack: tatters drift
-- left and stop the shots while she blows.
--
-- A missile that touches her wheel, her tower or her vane hurts her, at every
-- moment of the fight, her rise and her attacks included.

local STILL = { x = 0, y = 0 }

local GALE, BLADES, PUMP, VEIL, ARRIVAL, RAGE = 0, 1, 2, 3, 4, 5

-- The turn of her attacks, calm then enraged.
local CALM_TURN = { GALE, BLADES, PUMP }
local ENRAGED_TURN = { GALE, BLADES, VEIL, PUMP, BLADES, VEIL }

-- How far below her center her feet lie, so she stands on the ground under
-- her; the eye of her hub from her center, where her blades are thrown from.
local FEET_BELOW = 304
local EYE = { x = -32, y = -176 }

-- The five lanes of the band a gale blows along, the column a gust starts
-- from, and how many lanes it blows along.
local LANES = { 90, 260, 430, 600, 770 }
local GUST_COLUMN = 1300
local GUSTED_LANES = 2

-- Where the cracks of her pump open besides the one under the nearest ship:
-- two columns drawn between these; and how far above the ground the middle of
-- a geyser lies (half its 304 units).
local CRACK_LEFT, CRACK_RIGHT = 240, 1200
local CRACKS_ELSEWHERE = 2
local GEYSER_ABOVE = 152
local FLOOR_Y = 900

-- Where the tatters of her veil appear around her, from her center.
local VEIL_SPREAD = { x = -260, y = 200 }

-- The left edge her blades fly to, and the side of her hub the next blade
-- takes, above (-1) or below (1).
local BLADE_TURN_X = 60
local next_blade_side = 1

local function ground_at(x: number, otherwise: number): number
	local found, ground = pcall(level.ground, x)
	if not found or ground == nil then
		return otherwise
	end
	return ground
end

local function beat(attack: number, phase: string, ms: number)
	behaviours.wait(ms, { direction = STILL, starts_action = { attack = attack, phase = phase } })
end

-- Draws `count` of the lanes, each once.
local function lanes_drawn(count: number): { number }
	local left = table.clone(LANES)
	local drawn = {}
	for _ = 1, count do
		table.insert(drawn, table.remove(left, math.random(1, #left)))
	end
	return drawn
end

local function gale(settings)
	beat(GALE, "signal", settings.signal_ms)
	behaviours.wait(1, { direction = STILL, starts_action = { attack = GALE, phase = "strike" } })
	local lanes = lanes_drawn(GUSTED_LANES)
	local left = settings.gale_ms
	while left > 0 do
		for _, lane in lanes do
			behaviours.spawn("widow_gale", { x = GUST_COLUMN, y = lane })
		end
		behaviours.wait(settings.gust_gap_ms, { direction = STILL })
		left -= settings.gust_gap_ms
	end
	beat(GALE, "recover", settings.recover_ms)
end

local function blades(e, settings)
	beat(BLADES, "signal", settings.signal_ms)
	e = behaviours.wait(1, { direction = STILL, starts_action = { attack = BLADES, phase = "strike" } })
	local hub = { x = e.position.x + EYE.x, y = e.position.y + EYE.y }
	behaviours.spawn("widow_blade", hub)
	behaviours.spawn("widow_blade", hub)
	behaviours.wait(settings.blades_ms, { direction = STILL })
	beat(BLADES, "recover", settings.recover_ms)
end

-- The crack and its geyser appear together: the client hides then unveils
-- anything that appears inside the screen over SUMMONING_TIME (1 s), so the
-- geyser wells at the crack, harmless, until the crack has shown whole.
local function crack_at(x: number)
	local ground = ground_at(x, FLOOR_Y)
	behaviours.spawn("widow_crack", { x = x, y = ground })
	behaviours.spawn("widow_geyser", { x = x, y = ground - GEYSER_ABOVE })
end

local function pump(e, settings)
	behaviours.wait(1, { direction = STILL, starts_action = { attack = PUMP, phase = "signal" } })
	local ship = e.nearest_ship
	if ship ~= nil then
		crack_at(ship.x)
	end
	for _ = 1, CRACKS_ELSEWHERE do
		crack_at(math.random(CRACK_LEFT, CRACK_RIGHT))
	end
	behaviours.wait(settings.crack_ms, { direction = STILL })
	beat(PUMP, "strike", settings.pump_ms)
	beat(PUMP, "recover", settings.recover_ms)
end

local function veil(e, settings)
	beat(VEIL, "signal", settings.signal_ms)
	e = behaviours.wait(1, { direction = STILL, starts_action = { attack = VEIL, phase = "strike" } })
	for index = 1, settings.tatters do
		local share = (index - 1) / math.max(1, settings.tatters - 1)
		behaviours.spawn("widow_tatter", {
			x = e.position.x + VEIL_SPREAD.x,
			y = e.position.y - VEIL_SPREAD.y + 2 * VEIL_SPREAD.y * share,
		})
	end
	behaviours.wait(settings.veil_ms, { direction = STILL })
	beat(VEIL, "recover", settings.recover_ms)
end

behaviours.register("old_widow_turn", {
	{ name = "rest_y", lowest = 0, highest = 1080 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "rest_ms", lowest = 0, highest = 60000 },
	{ name = "enraged_rest_ms", lowest = 0, highest = 60000 },
	{ name = "signal_ms", lowest = 1, highest = 60000 },
	{ name = "gale_ms", lowest = 1, highest = 60000 },
	{ name = "gust_gap_ms", lowest = 1, highest = 60000 },
	{ name = "blades_ms", lowest = 1, highest = 60000 },
	{ name = "crack_ms", lowest = 1, highest = 60000 },
	{ name = "pump_ms", lowest = 1, highest = 60000 },
	{ name = "tatters", lowest = 1, highest = 16 },
	{ name = "veil_ms", lowest = 1, highest = 60000 },
	{ name = "recover_ms", lowest = 1, highest = 60000 },
	{ name = "calm_ms", lowest = 0, highest = 60000 },
}, function(e)
	local settings = e.settings
	local rest = { x = e.position.x, y = ground_at(e.position.x, settings.rest_y + FEET_BELOW) - FEET_BELOW }
	behaviours.wait(settings.rise_ms, {
		direction = STILL,
		jumps_to = rest,
		starts_action = { attack = ARRIVAL, phase = "signal" },
	})
	beat(ARRIVAL, "strike", settings.pump_ms)
	e = behaviours.wait(settings.calm_ms, { direction = STILL, ends_action = true })

	local raged = false
	local next_attack = 1
	while true do
		if e.enraged and not raged then
			raged = true
			next_attack = 1
			beat(RAGE, "enraged", settings.enraged_rest_ms)
		end
		local turn = if raged then ENRAGED_TURN else CALM_TURN
		local attack = turn[next_attack]
		next_attack = next_attack % #turn + 1
		if attack == GALE then
			gale(settings)
		elseif attack == BLADES then
			blades(e, settings)
		elseif attack == PUMP then
			pump(e, settings)
		else
			veil(e, settings)
		end
		e = behaviours.wait(if raged then settings.enraged_rest_ms else settings.rest_ms, {
			direction = STILL,
			ends_action = true,
		})
	end
end)

-- A blade flies a loop: out to the left edge on one side of the row it was
-- thrown from, back on the other side, in flight_ms, then it is gone. Each
-- tick it heads for where the loop puts it at its age.
behaviours.register("blade_boomerang", {
	{ name = "flight_ms", lowest = 1, highest = 60000 },
	{ name = "bulge", lowest = 0, highest = 1080 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.side == nil then
		data.side = next_blade_side
		next_blade_side = -next_blade_side
	end
	local reach = math.max(1, e.origin.x - BLADE_TURN_X)
	local share = (e.age_ms + 1000 / 60) / settings.flight_ms
	if share >= 1 and e.age_ms > 0 and e.origin.x > 0 then
		behaviours.destroy(e.id)
		return { direction = { x = -1, y = 0 } }
	end
	local angle = 2 * math.pi * math.min(share, 1)
	local target = {
		x = e.origin.x - reach * (1 - math.cos(angle)) / 2,
		y = e.origin.y + data.side * settings.bulge * math.sin(angle),
	}
	local x, y = target.x - e.position.x, target.y - e.position.y
	local step = e.top_speed / 60
	if x >= 0 and math.abs(x) < 1e-6 then
		x = -1e-3
	end
	return { direction = { x = math.clamp(x / step, -1, 1), y = math.clamp(y / step, -1, 1) } }
end)

-- The ground stays cracked for crack_ms, while its geyser wells, then
-- closes as the geyser bursts out of it.
behaviours.register("ground_crack", {
	{ name = "crack_ms", lowest = 1, highest = 60000 },
}, function(e)
	behaviours.wait(e.settings.crack_ms, { direction = STILL })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- A geyser wells at its crack for well_ms, rises for rise_ms, stands at its
-- full height for burst_ms, the only time it hurts (its sheet ties its hurt
-- zone to the strike), then falls back for fall_ms and is gone.
behaviours.register("geyser_burst", {
	{ name = "well_ms", lowest = 0, highest = 60000 },
	{ name = "rise_ms", lowest = 1, highest = 60000 },
	{ name = "burst_ms", lowest = 1, highest = 60000 },
	{ name = "fall_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	behaviours.wait(settings.well_ms, { direction = STILL })
	behaviours.wait(settings.rise_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	behaviours.wait(settings.burst_ms, { direction = STILL, starts_action = { attack = 0, phase = "strike" } })
	behaviours.wait(settings.fall_ms, { direction = STILL, starts_action = { attack = 0, phase = "recover" } })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- A tatter of the veil drifts left, swaying, and stops every missile it
-- meets; it is gone after life_ms.
behaviours.register("tatter_drift", {
	{ name = "sway", lowest = 0, highest = 1080 },
	{ name = "sway_ms", lowest = 1, highest = 60000 },
	{ name = "life_ms", lowest = 1, highest = 60000 },
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
	local target = data.row + settings.sway * math.sin(2 * math.pi * e.age_ms / settings.sway_ms)
	local step = (target - e.position.y) / (e.top_speed / 60)
	return { direction = { x = -1, y = math.clamp(step, -1, 1) } }
end)
