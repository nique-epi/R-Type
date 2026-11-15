#pragma once

#include <array>
#include <string_view>
#include "ShipLookConstants.hpp"

namespace rtype::client {

/**
 * @brief What the Bydo of the shop says, a Soviet trader of the station: in
 * capitals, without apostrophes, each line at most SHOPKEEPER_MAX_LINES lines
 * in his bubble. A word between stars is a word of the interface; {R} shows
 * the keycap of SHOP_RADIO_KEY.
 */

/** @brief One greeting when the shop opens, picked by how many items the
 * player owns, then where the radio is. */
constexpr std::array<std::string_view, 3> SHOPKEEPER_GREETINGS{
    "PRIVET, PILOT! WELCOME TO MY BOOTH. ONLY BEST GOODS OF MOTHER BYDO.",
    "AH, TOVARISCH! YOU HAVE KOPECKS? I HAVE HATS.",
    "COME, COME. NO POLICE HERE. ONLY HONEST BYDO BUSINESS."};
constexpr std::string_view SHOPKEEPER_RADIO_HINT =
    "TOO QUIET? CLICK MY *RADIO* OR PRESS {R}. IS GOOD RADIO.";

/** @brief The line of each ware bought, in the order of
 * identifiers::SHIP_LOOK_ITEM_NAMES. */
constexpr std::array<std::string_view, identifiers::SHIP_LOOK_ITEM_COUNT>
    SHOPKEEPER_BOUGHT_LINES{
        "TOP HAT! NOW YOU LOOK LIKE CAPITALIST. I FORGIVE YOU.",
        "PARTY HAT. PARTY IS AT MY HOUSE. BRING VODKA.",
        "A CROWN? THE TSAR IS BACK! DO NOT TELL COMRADES.",
        "PROPELLER CAP. NOW YOUR SHIP HAS TWO ENGINES.",
        "COWBOY HAT. YEEHAW, AS WE SAY IN SIBERIA.",
        "VIKING HELMET. COLD COUNTRY, GOOD PEOPLE.",
        "WIZARD HAT. MAGIC IS EXTRA. NO REFUNDS.",
        "CHEF HAT! YOU COOK BORSCHT FOR ME, DA?",
        "COOL SHADES. NOW BYDOS CANNOT SEE YOUR FEAR.",
        "MUSTACHE! FINALLY YOU LOOK LIKE MY UNCLE.",
        "CHAPKA! NOW YOU ARE TRUE COMRADE. ALMOST.",
        "TRICORN. PIRATE? I SAW NOTHING.",
        "HAT OF DED MOROZ. GIFTS NOT INCLUDED.",
        "HALO. VERY HOLY. VERY EXPENSIVE. VERY GOOD DEAL.",
        "HORNS. MY MOTHER IN LAW HAS SAME.",
        "SOMBRERO. FOR SUN. NO SUN IN SPACE, BUT OK.",
        "CONE FROM MOSCOW STREET. NOBODY WILL MISS IT.",
        "FEZ. I HAD ONE. I SOLD IT. TO YOU.",
        "DIPLOMA HAT. NOW YOU ARE ENGINEER. MOLODETS!",
        "ANTENNAE. NOW YOU CATCH RADIO MOSCOW.",
        "SAMURAI HELMET. TRADE FROM EAST. NO QUESTIONS.",
        "MOHAWK. VERY PUNK. VERY NOT SOVIET. I LIKE.",
        "3D GLASSES. NOW BYDOS COME OUT OF SCREEN.",
        "MONOCLE. VERY BOURGEOIS. I SAY NOTHING.",
        "BEARD OF RASPUTIN. VERY STRONG. HARD TO KILL.",
        "WAR SHARK. MANY TEETH. LIKE MY WIFE.",
        "VOSTOK! FIRST IN SPACE, LIKE GAGARIN. GOOD HULL.",
        "BYDO HUSK... IS MY COUSIN. HE IS OK WITH IT. PROBABLY.",
        "GOLDEN ACE. SHINY LIKE CATHEDRAL ROOF.",
        "GOLDEN BYDO! IS ME, BUT RICH. GOOD TASTE, PILOT."};

/** @brief A ware refused: too expensive, in turn; owned already; not for
 * sale. */
constexpr std::array<std::string_view, 3> SHOPKEEPER_TOO_EXPENSIVE_LINES{
    "NYET. NO KOPECKS, NO HAT.", "THIS IS SHOP, NOT CHARITY, TOVARISCH.",
    "SHOOT MORE BYDOS, THEN COME BACK. NOT ME, PLEASE."};
/** @brief A hull of the glass case the player cannot pay, one line for
 * each, in the order of the hulls of SHIP_LOOK_ITEM_NAMES: the war shark,
 * the vostok, the Bydo husk, the golden ace, the golden Bydo. */
constexpr std::array<std::string_view, identifiers::SHIP_HULL_COUNT>
    SHOPKEEPER_HULL_TOO_EXPENSIVE_LINES{
        "NO KOPECKS? THIS SHARK BITES WALLETS, NOT BYDOS.",
        "GAGARIN FLEW THIS FOR THE MOTHERLAND. YOU CANNOT EVEN PAY FOR IT.",
        "THIS IS MY COUSIN. HE IS NOT CHEAP. NEITHER AM I.",
        "REAL GOLD, COMRADE. YOU HAVE ONLY PAPER. COME BACK RICH.",
        "TEN THOUSAND KOPECKS. EVEN MY MOTHER CANNOT AFFORD THIS ONE."};
constexpr std::string_view SHOPKEEPER_ALREADY_YOURS_LINE =
    "YOU HAVE THIS ALREADY. TWO HATS, ONE HEAD? NYET.";
constexpr std::string_view SHOPKEEPER_NOT_FOR_SALE_LINE =
    "THIS? NOT FOR SALE. IS FOR DISPLAY. AND FOR ME.";

/** @brief A click on his abacus or his tea, the abacus and the tea
 * counted together: the first time, the second, then the third for every
 * click after it, once he gave up. */
constexpr std::array<std::string_view, 3> SHOPKEEPER_COUNTER_LINES{
    "MY ABACUS, MY TEA. YOU CAN TUNE THE RADIO. IS ALREADY A LOT.",
    "AGAIN? THE RADIO IS FOR YOU, TOVARISCH. NOT MY THINGS.",
    "FINE. TOUCH EVERYTHING. I AM TOO OLD FOR THIS."};

/** @brief A click on his bottles, in turn. */
constexpr std::array<std::string_view, 4> SHOPKEEPER_BOTTLES_LINES{
    "HANDS OFF! IS NOT FOR PILOTS. IS FOR ME.",
    "THIS ONE IS MEDICINE. VERY STRONG MEDICINE.",
    "NO TOUCH. LAST ONE WHO TOUCHED IS NOW IN SIBERIA.",
    "BOTTLES ARE NOT FOR SALE. BOTTLES ARE FOR WINTER."};

/** @brief The radio tuned to the theme of Papers, Please, in turn, and back
 * to the music of the shop. */
constexpr std::array<std::string_view, 4> SHOPKEEPER_BORDER_RADIO_LINES{
    "GLORY TO ARSTOTZKA! PAPERS, PLEASE. NO PAPERS? KOPECKS, PLEASE.",
    "AH, BORDER MUSIC. CAUSE NO TROUBLE IN MY SHOP.",
    "YOUR PAPERS ARE IN ORDER. YOUR WALLET, WE WILL SEE.",
    "ENTRY DENIED... JOKE! COME IN, BUY HAT."};
/** @brief The radio tuned to Kavorka, a happy acoustic music, busy and
 * frantic. */
constexpr std::string_view SHOPKEEPER_DANCE_RADIO_LINE =
    "SO HAPPY, SO FAST! MY TENTACLES CANNOT KEEP UP.";
constexpr std::string_view SHOPKEEPER_SHOP_MUSIC_LINE =
    "AH, BACK TO MOSCOW. MUCH BETTER.";

}  // namespace rtype::client
