--!strict
-- The level training_run: two stages a team plays one after the other, with
-- the enemies the game ships, placed in levels/training_run.ldtk. A server
-- plays it with `set level training_run`.
--
-- The first stage crosses the biome drift_fields: two groups of Bydos come in
-- as the level scrolls, then the scroll stops until the first Reaper is
-- destroyed. The second stage starts at a checkpoint, in the biome
-- ember_reach, and stops the scroll again on the last two Reapers: a team
-- that loses every life there starts that stage again, and only the enemies
-- placed after the checkpoint come back. The level is cleared once its last
-- stage is over and no enemy is left. The functions of `level` are described
-- on the page "Scripting enemies in Luau" of the docs.

-- The width of the playfield, in logical units: a placed enemy has come in
-- once the scroll brings its x this far.
local SCREEN_WIDTH = 1920

-- The scroll at which each placed Reaper has come in, in the order of the
-- level.
local function reaper_arrivals(): { number }
	local arrivals = {}
	for _, placed in level.placements() do
		if placed.sheet == "reaper" then
			table.insert(arrivals, placed.x - SCREEN_WIDTH)
		end
	end
	return arrivals
end

local function wait_for_a_clear_sky()
	while level.enemies() > 0 do
		level.wait(250)
	end
end

local function hold_the_scroll_at(arrival: number)
	level.wait_scroll(arrival)
	level.pause_scroll()
	wait_for_a_clear_sky()
	level.resume_scroll()
end

level.register("training_run", {
	function()
		level.biome("drift_fields")
		level.spawn_placed()
		hold_the_scroll_at(reaper_arrivals()[1])
		level.wait_scroll(1440)
	end,
	function()
		level.biome("ember_reach")
		hold_the_scroll_at(reaper_arrivals()[2])
	end,
}, { ldtk = "levels/training_run.ldtk" })
