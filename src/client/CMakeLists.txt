find_package(SFML 3 COMPONENTS Graphics Window System Audio CONFIG REQUIRED)

add_executable(r-type_client main.cpp)

target_link_libraries(r-type_client
  PRIVATE
    SFML::Graphics
    SFML::Audio
)

rtype_enable_warnings(r-type_client)
