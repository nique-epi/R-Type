target_sources(master_tests PRIVATE MasterDatabaseFile.cpp RunningMaster.cpp)

add_subdirectory(Http)
add_subdirectory(LaunchOptions)
add_subdirectory(Modules)
add_subdirectory(Servers)
