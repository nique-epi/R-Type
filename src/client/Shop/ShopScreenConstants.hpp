#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <string_view>
#include "SoundConstants.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief The sound of a purchase: the metal impact of the logo, the
 * nearest to a coin set down among the sounds of the client. */
constexpr const char* PURCHASE_SOUND_ID = LOGO_IMPACT_SOUND_ID;

/** @brief The radio of the counter (SHOP_RADIO_AREA): a click on it, or
 * SHOP_RADIO_KEY, changes the music between the music of the shop and the
 * music of the radio; under the pointer, the status line says so. */
constexpr sf::Keyboard::Key SHOP_RADIO_KEY = sf::Keyboard::Key::R;
constexpr Translation SHOP_RADIO_HINT{
    .english = "RADIO - CLICK OR R: CHANGE THE MUSIC",
    .french = "RADIO - CLIC OU R : CHANGER LA MUSIQUE"};

/** @brief The status line of the shop: a ware and its price, a ware the
 * player owns, a purchase, and why one was refused. */
constexpr std::string_view ITEM_MARK = "{ITEM}";
constexpr std::string_view PRICE_MARK = "{PRICE}";
constexpr std::string_view MISSING_MARK = "{MISSING}";
constexpr Translation BUY_HINT{
    .english = "{ITEM} - ENTER: BUY FOR {PRICE} KOPECKS",
    .french = "{ITEM} - ENTRÉE : ACHETER {PRICE} KOPECKS"};
constexpr Translation OWNED_HINT{.english = "{ITEM} - ALREADY YOURS",
                                 .french = "{ITEM} - DÉJÀ À VOUS"};
constexpr Translation BOUGHT_STATUS{.english = "BOUGHT: {ITEM}",
                                    .french = "ACHETÉ : {ITEM}"};
constexpr Translation ALREADY_YOURS_STATUS{.english = "ALREADY YOURS",
                                           .french = "DÉJÀ À VOUS"};
constexpr Translation NOT_FOR_SALE_STATUS{.english = "NOT FOR SALE",
                                          .french = "PAS À VENDRE"};
constexpr Translation NOT_ENOUGH_STATUS{
    .english = "NOT ENOUGH KOPECKS: {MISSING} MISSING",
    .french = "KOPECKS INSUFFISANTS : IL MANQUE {MISSING}"};
/** @brief What the status line says while the master is asked: the wallet
 * not read yet, a purchase not answered yet; and what the master answered
 * when it made no purchase for another reason than the price. */
constexpr Translation READING_WALLET_STATUS{
    .english = "READING YOUR WALLET...",
    .french = "LECTURE DE VOTRE PORTEFEUILLE..."};
constexpr Translation BUYING_STATUS{.english = "BUYING...",
                                    .french = "ACHAT..."};
constexpr Translation NOT_ENOUGH_KOPECKS_STATUS{
    .english = "NOT ENOUGH KOPECKS", .french = "KOPECKS INSUFFISANTS"};
constexpr Translation MASTER_SILENT_STATUS{
    .english = "NO ANSWER FROM THE MASTER",
    .french = "PAS DE RÉPONSE DU MAÎTRE"};
constexpr Translation MASTER_REFUSED_STATUS{.english = "THE MASTER REFUSED",
                                            .french = "LE MAÎTRE A REFUSÉ"};
constexpr Translation SESSION_ENDED_STATUS{
    .english = "SIGN IN AGAIN TO BUY",
    .french = "RECONNECTEZ-VOUS POUR ACHETER"};

}  // namespace rtype::client
