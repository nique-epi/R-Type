add_library(rtype_client_shop_speech STATIC ShopkeeperSpeech.cpp)

target_include_directories(rtype_client_shop_speech PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shop_speech
  PUBLIC
    rtype_client_audio_contract_service
    rtype_client_economy
    rtype_client_spoken_line
    rtype_engine_time
  PRIVATE
    rtype_client_shop_painting
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_shop_speech)
