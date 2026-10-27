add_library(rtype_client_typing STATIC Typing.cpp)

target_include_directories(rtype_client_typing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_typing
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_engine_time
  PRIVATE
    SFML::System
)

rtype_enable_warnings(rtype_client_typing)
