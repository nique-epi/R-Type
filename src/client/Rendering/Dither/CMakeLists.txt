add_library(rtype_client_dither STATIC BayerThreshold.cpp)

target_include_directories(rtype_client_dither PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_dither PUBLIC SFML::System)

rtype_enable_warnings(rtype_client_dither)
