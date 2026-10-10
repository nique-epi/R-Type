add_library(rtype_client_gaussian_blur STATIC GaussianBlur.cpp)

target_include_directories(rtype_client_gaussian_blur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_gaussian_blur
  PUBLIC
    rtype_client_background
    rtype_logging
  PRIVATE
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_gaussian_blur)
