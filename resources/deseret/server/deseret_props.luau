--!strict
-- The decor of Deseret that does something, laid on top of the maps of its
-- biomes. The maps are only a picture: nothing painted on them can burst or
-- roll, so each of these is a body the level creates past the right edge,
-- which the scroll then carries.
--
-- What sets the decor of Deseret apart from the toxic barrel of the Ashlands
-- is its visible fuse: hit, a keg or a crate does not burst at once, it
-- warns; and its blast lights the kegs around it, a chain the players see
-- coming and may turn against the mobs. A crate also fells a spire in the
-- open, or knocks a prop off the ceiling of the mine. A mine cart shot from
-- behind rolls off and crushes the mobs ahead of it.

local STILL = { x = 0, y = 0 }
local RIGHT = { x = 1, y = 0 }
local LEFT = { x = -1, y = 0 }

-- How fast the ground scrolls left, in logical units per second
-- (LEVEL_SCROLL_SPEED): a cart at rest moves with it.
local SCROLL_SPEED = 120

-- The blasts, by `blast` of the sheet: the body that hurts the mobs and the
-- one that hurts the ships.
local BLASTS = {
	[0] = { mobs = "keg_blast", ships = "keg_heat" },
	[1] = { mobs = "dynamite_blast", ships = "dynamite_heat" },
}

-- Where the dust of a felled spire falls from (its top at the top of the
-- playfield), how far around the column of the crate its rocks fall, and how
-- far under the ceiling a prop of the mine hangs from.
local SPIRE_DUST_ROW = 96
local ROCK_SPREAD = 48
local ROCK_ROW = 0
local PROP_DROP = 68

-- How far ahead of a rolling cart its ram lies, and how many of its steps
-- the ram waits for a cart that wrote nothing before it is gone.
local RAM_AHEAD = 48
local RAM_PATIENCE = 3

-- The blasts on screen, by network id: a body that swallows missiles lets
-- them through, so one blast reaches every body around it.
local blasts: { [number]: boolean } = {}

-- The explosives on screen, by network id: where each stands, and whether a
-- blast lit it. Each writes itself every tick and forgets itself past the
-- left edge.
local explosives: { [number]: { x: number, y: number, lit: boolean } } = {}

-- The carts that roll, by network id: where each is, and a count of its
-- steps, which its ram reads to tell it is still there; and the cart the
-- next ram follows.
local carts: { [number]: { x: number, y: number, steps: number } } = {}
local next_ram_cart: number? = nil

local function burst(kind: number, place: { x: number, y: number })
	local blast = BLASTS[kind]
	blasts[behaviours.spawn(blast.mobs, place)] = true
	behaviours.spawn(blast.ships, place)
end

-- Lights every explosive within `reach` of `place`.
local function light_around(place: { x: number, y: number }, reach: number)
	for _, other in explosives do
		local x, y = other.x - place.x, other.y - place.y
		if x * x + y * y <= reach * reach then
			other.lit = true
		end
	end
end

-- What a crate brings down as it bursts: a prop off the ceiling over it in
-- the mine, a spire across a corridor of rocks in the open.
local function bring_down(place: { x: number, y: number })
	local found, ceiling = pcall(level.ceiling, place.x)
	if found and ceiling ~= nil then
		behaviours.spawn("mine_prop", { x = place.x, y = ceiling + PROP_DROP })
		return
	end
	behaviours.spawn("spire_dust", { x = place.x, y = SPIRE_DUST_ROW })
end

-- Lets a hit through when `through()` says so; otherwise refuses it, and
-- swallows the missile unless it is a blast. Returns whether it was a hit of
-- a player.
local function judged_hit(hit, through: () -> boolean): (boolean, boolean)
	if through() then
		return true, false
	end
	if blasts[hit.missile] then
		return false, false
	end
	behaviours.destroy(hit.missile)
	return false, true
end

-- A keg or a crate: it stands, writing where it is. A hit lights it, and so
-- does a blast close enough; its fuse burns fuse_ms, then it bursts on
-- itself, which kills it in its own blast, lights the explosives within
-- light_reach, and, for a crate, brings something down.
behaviours.register("fuse_watch", {
	{ name = "fuse_ms", lowest = 1, highest = 60000 },
	{ name = "blast", lowest = 0, highest = 1 },
	{ name = "light_reach", lowest = 0, highest = 1920 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.seen == nil then
		data.seen = true
		behaviours.pre_hit(function(hit)
			local through, by_a_player = judged_hit(hit, function()
				return data.bursting == true
			end)
			if by_a_player or blasts[hit.missile] then
				data.lit = true
			end
			return through
		end)
	end
	if e.position.x < 0 then
		explosives[e.id] = nil
		return { direction = STILL }
	end
	local entry = explosives[e.id]
	if entry == nil then
		entry = { x = e.position.x, y = e.position.y, lit = false }
		explosives[e.id] = entry
	end
	entry.x, entry.y = e.position.x, e.position.y
	if data.bursting or not (data.lit or entry.lit) then
		return { direction = STILL }
	end
	e = behaviours.wait(settings.fuse_ms, { direction = STILL, starts_action = { attack = 0, phase = "signal" } })
	data.bursting = true
	explosives[e.id] = nil
	local place = { x = e.position.x, y = e.position.y }
	burst(settings.blast, place)
	light_around(place, settings.light_reach)
	if settings.blast == 1 then
		bring_down(place)
	end
	return { direction = STILL }
end)

-- A blast that hurts the mobs lives life_ms, the burst of the death of
-- what blew up, then is gone. It heads right, the edge that removes a missile
-- of a player, at the one unit per second of its sheet.
behaviours.register("blast_fade", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	if e.age_ms >= e.settings.life_ms then
		blasts[e.id] = nil
		behaviours.destroy(e.id)
	end
	return { direction = RIGHT }
end)

-- The same for a blast that hurts the ships, heading left, the edge that
-- removes a missile of an enemy.
behaviours.register("heat_fade", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	if e.age_ms >= e.settings.life_ms then
		behaviours.destroy(e.id)
	end
	return { direction = LEFT }
end)

-- A fire, or a geyser, burns life_ms, then is gone.
behaviours.register("fading_fire", {
	{ name = "life_ms", lowest = 1, highest = 60000 },
}, function(e)
	behaviours.wait(e.settings.life_ms, { direction = STILL })
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- The dust of a felled spire falls warn_ms, then rocks fall along its
-- column for fall_ms, one every rock_gap_ms, and it is gone.
behaviours.register("spire_fall", {
	{ name = "warn_ms", lowest = 1, highest = 60000 },
	{ name = "fall_ms", lowest = 1, highest = 60000 },
	{ name = "rock_gap_ms", lowest = 1, highest = 60000 },
}, function(e)
	local settings = e.settings
	e = behaviours.wait(settings.warn_ms, { direction = STILL })
	local left = settings.fall_ms
	while left > 0 do
		behaviours.spawn("spire_rock", { x = e.position.x + math.random(-ROCK_SPREAD, ROCK_SPREAD), y = ROCK_ROW })
		e = behaviours.wait(settings.rock_gap_ms, { direction = STILL })
		left -= settings.rock_gap_ms
	end
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- A mine cart rests on its rails, moving with the ground. The first hit sets
-- it rolling right at its top speed, its ram ahead of it, for roll_ms; a
-- laden cart then bursts its keg on itself, and dies in its blast; an empty
-- one is gone.
behaviours.register("cart_roll", {
	{ name = "roll_ms", lowest = 1, highest = 60000 },
	{ name = "laden", lowest = 0, highest = 1 },
}, function(e)
	local settings = e.settings
	local data = e.data
	if data.seen == nil then
		data.seen = true
		behaviours.pre_hit(function(hit)
			local through = judged_hit(hit, function()
				return data.bursting == true
			end)
			data.rolling = true
			return through
		end)
	end
	if not data.rolling then
		return { direction = { x = -SCROLL_SPEED / e.top_speed, y = 0 } }
	end
	if data.rolled_ms == nil then
		data.rolled_ms = 0
		next_ram_cart = e.id
		behaviours.spawn("cart_ram", { x = e.position.x + RAM_AHEAD, y = e.position.y })
	end
	local entry = carts[e.id] or { x = 0, y = 0, steps = 0 }
	entry.x, entry.y, entry.steps = e.position.x, e.position.y, entry.steps + 1
	carts[e.id] = entry
	data.rolled_ms += e.elapsed_ms
	if data.rolled_ms < settings.roll_ms then
		return { direction = RIGHT }
	end
	carts[e.id] = nil
	if settings.laden == 1 then
		if not data.bursting then
			data.bursting = true
			burst(0, { x = e.position.x, y = e.position.y })
		end
		return { direction = STILL }
	end
	behaviours.destroy(e.id)
	return { direction = STILL }
end)

-- The ram of a rolling cart keeps RAM_AHEAD in front of it, and is gone with
-- it. With no cart to follow it heads right, the edge that removes a missile
-- of a player.
behaviours.register("cart_ram", {}, function(e)
	local data = e.data
	if data.steps == nil then
		data.cart = next_ram_cart
		next_ram_cart = nil
		data.steps, data.waited = -1, 0
	end
	if data.cart == nil then
		return { direction = RIGHT }
	end
	local cart = carts[data.cart]
	if cart == nil or cart.steps == data.steps then
		data.waited += 1
		if cart == nil or data.waited >= RAM_PATIENCE then
			behaviours.destroy(e.id)
		end
		return { direction = RIGHT }
	end
	data.steps, data.waited = cart.steps, 0
	return { direction = RIGHT, jumps_to = { x = cart.x + RAM_AHEAD, y = cart.y } }
end)
