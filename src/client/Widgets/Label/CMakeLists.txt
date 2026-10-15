add_library(rtype_client_label STATIC Label.cpp)

target_include_directories(rtype_client_label PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_label PUBLIC rtype_client_widgets)

rtype_enable_warnings(rtype_client_label)
