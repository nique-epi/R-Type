if(NOT RTYPE_MODULE_PLUGINS)
  return()
endif()

if(WIN32)
  set(RTYPE_SHARED_LIBRARY_SOURCE SharedLibraryWindows.cpp)
else()
  set(RTYPE_SHARED_LIBRARY_SOURCE SharedLibraryPosix.cpp)
endif()

add_library(rtype_engine_shared_library STATIC ${RTYPE_SHARED_LIBRARY_SOURCE})

target_include_directories(rtype_engine_shared_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_shared_library
  PUBLIC
    rtype_engine_exceptions
    rtype_logging_path_text
    ${CMAKE_DL_LIBS}
)

target_compile_definitions(rtype_engine_shared_library
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_engine_shared_library)
