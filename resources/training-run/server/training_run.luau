--!strict
-- The level training_run: two stages a team plays one after the other, with
-- the enemies the game ships. A server plays it with `set level training_run`.
--
-- The first stage crosses open space, with the stars alone: two groups of
-- Bydos come as the level scrolls, then the scroll stops until a Reaper is
-- destroyed. The second stage starts at a checkpoint, in the biome fosse,
-- entered through its cave: a team that loses every life there
-- starts that stage again. The level is cleared once
-- its last stage is over and no enemy is left. The functions of `level` are
-- described on the page "Scripting enemies in Luau" of the docs.

-- Past the right edge of the playfield, where a Bydo enters, and the row in
-- the middle of it, in logical units.
local ENTRY_COLUMN = 1984
local MIDDLE_ROW = 540

local function send(sheet: string, rows: { number })
	for _, row in rows do
		behaviours.spawn(sheet, { x = ENTRY_COLUMN, y = row })
	end
end

local function wait_for_a_clear_sky()
	while level.enemies() > 0 do
		level.wait(250)
	end
end

level.register("training_run", {
	function()
		level.wait_scroll(240)
		send("drifter", { 300, MIDDLE_ROW, 780 })
		level.wait_scroll(720)
		send("weaver", { 400, 680 })
		level.wait_scroll(1200)
		level.pause_scroll()
		send("reaper", { MIDDLE_ROW })
		wait_for_a_clear_sky()
		level.resume_scroll()
		level.wait_scroll(1440)
	end,
	function()
		level.biome("fosse")
		level.wait_scroll(1680)
		send("drifter", { 240, 420, 660, 840 })
		level.wait_scroll(2160)
		send("voltjelly", { 360, 720 })
		send("weaver", { MIDDLE_ROW })
		level.wait_scroll(2640)
		level.pause_scroll()
		send("reaper", { 360, 720 })
		wait_for_a_clear_sky()
		level.resume_scroll()
	end,
})
