add_library(rtype_client_item_list STATIC ItemList.cpp)

target_include_directories(rtype_client_item_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_item_list
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_outline
)

rtype_enable_warnings(rtype_client_item_list)
