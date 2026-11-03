find_package(SQLiteCpp CONFIG REQUIRED)

add_subdirectory(Exceptions)

add_library(rtype_storage STATIC DatabaseFile.cpp)

target_include_directories(rtype_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_storage
  PUBLIC
    SQLiteCpp
    rtype_storage_exceptions
)

rtype_enable_warnings(rtype_storage)
