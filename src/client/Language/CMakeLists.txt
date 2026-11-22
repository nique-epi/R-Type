add_library(rtype_client_language STATIC TextMarks.cpp)

target_include_directories(rtype_client_language PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

rtype_enable_warnings(rtype_client_language)
