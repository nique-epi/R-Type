-- The script of the bench with coroutines: it never returns, and the wait of
-- one tick resumes it at every tick, the most a coroutine can cost.

behaviours.register("bench_waiting", {
	{ name = "cruise_speed", lowest = 0, highest = 1920 },
}, function(view)
	while true do
		view = behaviours.wait(16, {
			direction = { x = -1, y = 0 },
			top_speed = view.settings.cruise_speed,
			shooting = true,
		})
	end
end)
