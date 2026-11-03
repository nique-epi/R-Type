add_subdirectory(Tabs)
add_subdirectory(Screen)

add_library(rtype_client_options INTERFACE)

target_link_libraries(rtype_client_options
  INTERFACE
    rtype_client_options_screen
    rtype_client_options_tabs
)
