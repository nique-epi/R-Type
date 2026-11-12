add_library(rtype_client_debug_journal STATIC
  LagometerJournal.cpp
  LagometerJournalText.cpp
)

target_include_directories(rtype_client_debug_journal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_debug_journal
  PUBLIC
    rtype_client_debug_panel
)

rtype_enable_warnings(rtype_client_debug_journal)
