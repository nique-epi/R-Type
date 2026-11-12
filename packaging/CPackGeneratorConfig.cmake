# Read by cpack once per generator (CPACK_PROJECT_CONFIG_FILE). An archive
# keeps a top folder named after it; a disk image holds R-Type.app at its
# root, next to the link to Applications, and an installer holds bin/ at the
# root of its install folder, where its Start menu shortcut looks for the
# executable.
if(CPACK_GENERATOR STREQUAL "ZIP" OR CPACK_GENERATOR STREQUAL "TGZ")
  set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
else()
  set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
endif()
