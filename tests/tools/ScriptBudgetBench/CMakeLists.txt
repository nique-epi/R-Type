add_library(rtype_tools_script_budget_bench STATIC
  BenchContentFolders.cpp
  BenchLaunchOptions.cpp
  BenchScene.cpp
  TickFigures.cpp
  TimedBehaviour.cpp
)

target_include_directories(rtype_tools_script_budget_bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_script_budget_bench
  PUBLIC
    rtype_game
    rtype_game_scripting
    rtype_server_launch_options
)

target_compile_definitions(rtype_tools_script_budget_bench PRIVATE RTYPE_BENCH_REPOSITORY_ROOT="${PROJECT_SOURCE_DIR}")

rtype_enable_warnings(rtype_tools_script_budget_bench)

add_executable(script_budget_bench main.cpp)

target_link_libraries(script_budget_bench PRIVATE rtype_tools_script_budget_bench)

set_target_properties(script_budget_bench PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

rtype_enable_warnings(script_budget_bench)

rtype_add_test(NAME script_budget_bench_tests SOURCES BenchLaunchOptionsTest.cpp BenchSceneTest.cpp TickFiguresTest.cpp LIBS rtype_tools_script_budget_bench)
