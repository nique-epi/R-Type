--!strict
-- The behaviour of the Crimson Drifter: it crosses the playfield at its
-- cruise speed, and when a ship comes level with it, within align_range
-- logical units, it charges at that ship at its charge speed. Its sheet,
-- entities/crimson_drifter.toml, names it with pilot = "crimson_charge" and
-- gives the three settings. The view and the intent are described on the page
-- "Scripting enemies in Luau" of the docs.

behaviours.register("crimson_charge", {
	{ name = "align_range", lowest = 0, highest = 1080 },
	{ name = "cruise_speed", lowest = 0, highest = 1920 },
	{ name = "charge_speed", lowest = 0, highest = 1920 },
}, function(view)
	local settings = view.settings
	local ship = view.nearest_ship
	local level = ship ~= nil and math.abs(ship.y - view.position.y) <= settings.align_range
	if not level then
		return {
			direction = { x = -1, y = 0 },
			top_speed = settings.cruise_speed,
			shooting = true,
		}
	end
	return {
		direction = { x = -1, y = if ship.y > view.position.y then 0.25 else -0.25 },
		top_speed = settings.charge_speed,
		shooting = true,
	}
end)
