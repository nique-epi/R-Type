# Read by cpack once per generator (CPACK_PROJECT_CONFIG_FILE). An archive
# keeps a top folder named after it; a disk image holds R-Type.app at its
# root, next to the link to Applications.
if(CPACK_GENERATOR STREQUAL "DragNDrop")
  set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
else()
  set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
endif()
