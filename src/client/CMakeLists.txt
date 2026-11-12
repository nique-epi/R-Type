find_package(SFML 3 COMPONENTS Graphics Window System Audio CONFIG REQUIRED)

add_library(rtype_client_constants INTERFACE)

target_include_directories(rtype_client_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_constants INTERFACE rtype_network_constants)

add_subdirectory(Exceptions)
add_subdirectory(Logger)
add_subdirectory(Platform)
add_subdirectory(Assets)
add_subdirectory(Input)
add_subdirectory(Rendering)
add_subdirectory(Replication)
add_subdirectory(Prediction)
add_subdirectory(Window)
add_subdirectory(Settings)
add_subdirectory(Audio)
add_subdirectory(Widgets)
add_subdirectory(Screens)
add_subdirectory(Update)
add_subdirectory(Menus)
add_subdirectory(Lobby)
add_subdirectory(Hangar)
add_subdirectory(Game)
add_subdirectory(Application)
add_subdirectory(Connection)
add_subdirectory(Metrics)
add_subdirectory(Debug)
add_subdirectory(LaunchOptions)
add_subdirectory(Modules)

add_executable(r-type_client main.cpp)

target_link_libraries(r-type_client
  PRIVATE
    rtype_client_constants
    rtype_client_modules
    rtype_client_rendering
    rtype_logging
)

# In Release, the Windows client is a GUI program: no console window opens
# next to the game. Its entry point stays main(), which the console runtime
# start-up calls.
set_target_properties(r-type_client PROPERTIES
  WIN32_EXECUTABLE $<CONFIG:Release>
)
target_link_options(r-type_client PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/ENTRY:mainCRTStartup>
)

rtype_enable_warnings(r-type_client)
