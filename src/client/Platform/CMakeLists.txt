if(WIN32)
  set(RTYPE_PLATFORM_SOURCES ReducedMotionWindows.cpp HighContrastWindows.cpp SettingsFolderWindows.cpp BorderlessScreenWindows.cpp JournalFolderNextToExecutable.cpp)
elseif(APPLE)
  set(RTYPE_PLATFORM_SOURCES ReducedMotionMacOs.cpp HighContrastMacOs.cpp MacOsWorkspaceFlag.cpp SettingsFolderMacOs.cpp BorderlessScreenMacOs.cpp JournalFolderMacOs.cpp MacOsUserFolder.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(RTYPE_PLATFORM_SOURCES ReducedMotionLinux.cpp HighContrastLinux.cpp CommandFirstLine.cpp SettingsFolderLinux.cpp BorderlessScreenLinux.cpp JournalFolderNextToExecutable.cpp)
else()
  message(FATAL_ERROR "The client reads the system on Windows, macOS and Linux only")
endif()

add_library(rtype_client_platform STATIC ${RTYPE_PLATFORM_SOURCES})

target_include_directories(rtype_client_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_client_platform
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

if(APPLE)
  target_link_libraries(rtype_client_platform PRIVATE "-framework AppKit" objc)
endif()

if(WIN32)
  target_link_libraries(rtype_client_platform PRIVATE shell32 ole32 uuid)
endif()

rtype_enable_warnings(rtype_client_platform)
