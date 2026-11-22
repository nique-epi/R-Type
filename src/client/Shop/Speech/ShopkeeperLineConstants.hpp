#pragma once

#include <array>
#include "ShipLookConstants.hpp"
#include "Translation.hpp"

namespace rtype::client {

/**
 * @brief What the Bydo of the shop says, a Soviet trader of the station: in
 * capitals, without apostrophes in either language, his French as broken as
 * his English, each line at most SHOPKEEPER_MAX_LINES lines
 * in his bubble. A word between stars is a word of the interface; {R} shows
 * the keycap of SHOP_RADIO_KEY.
 */

/** @brief One greeting when the shop opens, picked by how many items the
 * player owns, then where the radio is. */
constexpr std::array<Translation, 3> SHOPKEEPER_GREETINGS{
    Translation{
        .english = "PRIVET, PILOT! WELCOME TO MY BOOTH. ONLY BEST GOODS OF "
                   "MOTHER BYDO.",
        .french = "PRIVET, PILOTE ! ICI MEILLEURS ARTICLES DE MÈRE BYDO."},
    Translation{
        .english = "AH, TOVARISCH! YOU HAVE KOPECKS? I HAVE HATS.",
        .french = "AH, TOVARISCH ! TOI AVOIR KOPECKS ? MOI AVOIR CHAPEAUX."},
    Translation{
        .english = "COME, COME. NO POLICE HERE. ONLY HONEST BYDO BUSINESS.",
        .french =
            "VIENS, VIENS. PAS POLICE ICI. SEULEMENT HONNÊTE COMMERCE BYDO."}};
constexpr Translation SHOPKEEPER_RADIO_HINT{
    .english = "TOO QUIET? CLICK MY *RADIO* OR PRESS {R}. IS GOOD RADIO.",
    .french =
        "TROP CALME ? CLIQUE MA *RADIO* OU APPUIE {R}. ÊTRE BONNE RADIO."};

/** @brief The line of each ware bought, in the order of
 * identifiers::SHIP_LOOK_ITEM_NAMES. */
constexpr std::array<Translation, identifiers::SHIP_LOOK_ITEM_COUNT>
    SHOPKEEPER_BOUGHT_LINES{
        Translation{
            .english = "TOP HAT! NOW YOU LOOK LIKE CAPITALIST. I FORGIVE YOU.",
            .french =
                "HAUT-DE-FORME ! TOI AVOIR AIR CAPITALISTE. MOI PARDONNER."},
        Translation{
            .english = "PARTY HAT. PARTY IS AT MY HOUSE. BRING VODKA.",
            .french = "CHAPEAU DE FÊTE. FÊTE ÊTRE CHEZ MOI. APPORTE VODKA."},
        Translation{
            .english = "A CROWN? THE TSAR IS BACK! DO NOT TELL COMRADES.",
            .french = "UNE COURONNE ? LE TSAR EST DE RETOUR ! PAS DIRE AUX "
                      "CAMARADES."},
        Translation{.english = "PROPELLER CAP. NOW YOUR SHIP HAS TWO ENGINES.",
                    .french = "CASQUETTE À HÉLICE. MAINTENANT TON VAISSEAU "
                              "AVOIR DEUX MOTEURS."},
        Translation{
            .english = "COWBOY HAT. YEEHAW, AS WE SAY IN SIBERIA.",
            .french = "CHAPEAU DE COW-BOY. YIHAA, COMME ON DIT EN SIBÉRIE."},
        Translation{.english = "VIKING HELMET. COLD COUNTRY, GOOD PEOPLE.",
                    .french = "CASQUE VIKING. PAYS FROID, BONS GENS."},
        Translation{
            .english = "WIZARD HAT. MAGIC IS EXTRA. NO REFUNDS.",
            .french =
                "CHAPEAU DE SORCIER. MAGIE EN SUPPLÉMENT. PAS REMBOURSER."},
        Translation{.english = "CHEF HAT! YOU COOK BORSCHT FOR ME, DA?",
                    .french = "TOQUE ! TOI CUISINER BORTSCH POUR MOI, DA ?"},
        Translation{
            .english = "COOL SHADES. NOW BYDOS CANNOT SEE YOUR FEAR.",
            .french = "LUNETTES CLASSE. MAINTENANT BYDOS PAS VOIR TA PEUR."},
        Translation{.english = "MUSTACHE! FINALLY YOU LOOK LIKE MY UNCLE.",
                    .french = "MOUSTACHE ! ENFIN TOI RESSEMBLER MON ONCLE."},
        Translation{
            .english = "CHAPKA! NOW YOU ARE TRUE COMRADE. ALMOST.",
            .french = "CHAPKA ! MAINTENANT TOI VRAI CAMARADE. PRESQUE."},
        Translation{.english = "TRICORN. PIRATE? I SAW NOTHING.",
                    .french = "TRICORNE. PIRATE ? MOI RIEN VU."},
        Translation{.english = "HAT OF DED MOROZ. GIFTS NOT INCLUDED.",
                    .french = "CHAPEAU DE DED MOROZ. CADEAUX PAS COMPRIS."},
        Translation{
            .english = "HALO. VERY HOLY. VERY EXPENSIVE. VERY GOOD DEAL.",
            .french = "AURÉOLE. TRÈS SAINTE. TRÈS CHÈRE. TRÈS BONNE AFFAIRE."},
        Translation{.english = "HORNS. MY MOTHER IN LAW HAS SAME.",
                    .french = "CORNES. MA BELLE-MÈRE AVOIR LES MÊMES."},
        Translation{
            .english = "SOMBRERO. FOR SUN. NO SUN IN SPACE, BUT OK.",
            .french =
                "SOMBRERO. POUR SOLEIL. PAS SOLEIL DANS ESPACE, MAIS OK."},
        Translation{.english = "CONE FROM MOSCOW STREET. NOBODY WILL MISS IT.",
                    .french = "CÔNE DE RUE DE MOSCOU. PERSONNE VA REGRETTER."},
        Translation{.english = "FEZ. I HAD ONE. I SOLD IT. TO YOU.",
                    .french = "FEZ. MOI EN AVOIR UN. MOI VENDU. À TOI."},
        Translation{
            .english = "DIPLOMA HAT. NOW YOU ARE ENGINEER. MOLODETS!",
            .french = "TOQUE DE DIPLÔMÉ. MAINTENANT TOI INGÉNIEUR. MOLODETS !"},
        Translation{.english = "ANTENNAE. NOW YOU CATCH RADIO MOSCOW.",
                    .french = "ANTENNES. MAINTENANT TOI CAPTER RADIO MOSCOU."},
        Translation{
            .english = "SAMURAI HELMET. TRADE FROM EAST. NO QUESTIONS.",
            .french =
                "CASQUE DE SAMOURAÏ. COMMERCE VENU DE EST. PAS QUESTIONS."},
        Translation{
            .english = "MOHAWK. VERY PUNK. VERY NOT SOVIET. I LIKE.",
            .french =
                "CRÊTE IROQUOISE. TRÈS PUNK. TRÈS PAS SOVIÉTIQUE. MOI AIMER."},
        Translation{.english = "3D GLASSES. NOW BYDOS COME OUT OF SCREEN.",
                    .french = "LUNETTES 3D. MAINTENANT BYDOS SORTIR DE ÉCRAN."},
        Translation{.english = "MONOCLE. VERY BOURGEOIS. I SAY NOTHING.",
                    .french = "MONOCLE. TRÈS BOURGEOIS. MOI RIEN DIRE."},
        Translation{
            .english = "BEARD OF RASPUTIN. VERY STRONG. HARD TO KILL.",
            .french = "BARBE DE RASPOUTINE. TRÈS FORTE. DIFFICILE À TUER."},
        Translation{
            .english = "WAR SHARK. MANY TEETH. LIKE MY WIFE.",
            .french = "REQUIN DE GUERRE. BEAUCOUP DENTS. COMME MA FEMME."},
        Translation{
            .english = "VOSTOK! FIRST IN SPACE, LIKE GAGARIN. GOOD HULL.",
            .french =
                "VOSTOK ! PREMIER DANS ESPACE, COMME GAGARINE. BONNE COQUE."},
        Translation{
            .english = "BYDO HUSK... IS MY COUSIN. HE IS OK WITH IT. PROBABLY.",
            .french = "CARCASSE BYDO... ÊTRE MON COUSIN. LUI ÊTRE OK AVEC ÇA. "
                      "PROBABLEMENT."},
        Translation{.english = "GOLDEN ACE. SHINY LIKE CATHEDRAL ROOF.",
                    .french = "AS EN OR. BRILLANT COMME TOIT DE CATHÉDRALE."},
        Translation{
            .english = "GOLDEN BYDO! IS ME, BUT RICH. GOOD TASTE, PILOT.",
            .french = "BYDO EN OR ! ÊTRE MOI, MAIS RICHE. BON GOÛT, PILOTE."}};

/** @brief A ware refused: too expensive, in turn; owned already; not for
 * sale. */
constexpr std::array<Translation, 3> SHOPKEEPER_TOO_EXPENSIVE_LINES{
    Translation{.english = "NYET. NO KOPECKS, NO HAT.",
                .french = "NYET. PAS KOPECKS, PAS CHAPEAU."},
    Translation{.english = "THIS IS SHOP, NOT CHARITY, TOVARISCH.",
                .french = "ICI ÊTRE BOUTIQUE, PAS CHARITÉ, TOVARISCH."},
    Translation{
        .english = "SHOOT MORE BYDOS, THEN COME BACK. NOT ME, PLEASE.",
        .french = "TIRE PLUS BYDOS, PUIS REVIENS. PAS SUR MOI, PITIÉ."}};
/** @brief A hull of the glass case the player cannot pay, one line for
 * each, in the order of the hulls of SHIP_LOOK_ITEM_NAMES: the war shark,
 * the vostok, the Bydo husk, the golden ace, the golden Bydo. */
constexpr std::array<Translation, identifiers::SHIP_HULL_COUNT>
    SHOPKEEPER_HULL_TOO_EXPENSIVE_LINES{
        Translation{
            .english = "NO KOPECKS? THIS SHARK BITES WALLETS, NOT BYDOS.",
            .french = "PAS KOPECKS ? CE REQUIN MORD PORTEFEUILLES, PAS BYDOS."},
        Translation{.english = "GAGARIN FLEW THIS FOR THE MOTHERLAND. YOU "
                               "CANNOT EVEN PAY FOR IT.",
                    .french = "GAGARINE AVOIR VOLÉ ÇA POUR LA PATRIE. TOI MÊME "
                              "PAS POUVOIR PAYER."},
        Translation{
            .english = "THIS IS MY COUSIN. HE IS NOT CHEAP. NEITHER AM I.",
            .french = "ÇA ÊTRE MON COUSIN. LUI PAS BON MARCHÉ. MOI NON PLUS."},
        Translation{
            .english =
                "REAL GOLD, COMRADE. YOU HAVE ONLY PAPER. COME BACK RICH.",
            .french = "VRAI OR, CAMARADE. TOI AVOIR SEULEMENT PAPIER. REVIENS "
                      "RICHE."},
        Translation{
            .english =
                "TWENTY THOUSAND KOPECKS. EVEN MY MOTHER CANNOT AFFORD IT.",
            .french =
                "VINGT MILLE KOPECKS. MÊME MA MÈRE PAS POUVOIR PAYER ÇA."}};
constexpr Translation SHOPKEEPER_ALREADY_YOURS_LINE{
    .english = "YOU HAVE THIS ALREADY. TWO HATS, ONE HEAD? NYET.",
    .french = "TOI AVOIR DÉJÀ ÇA. DEUX CHAPEAUX, UNE TÊTE ? NYET."};
constexpr Translation SHOPKEEPER_NOT_FOR_SALE_LINE{
    .english = "THIS? NOT FOR SALE. IS FOR DISPLAY. AND FOR ME.",
    .french = "ÇA ? PAS À VENDRE. ÊTRE POUR VITRINE. ET POUR MOI."};

/** @brief A click on his abacus or his tea, the abacus and the tea
 * counted together: the first time, the second, then the third for every
 * click after it, once he gave up. */
constexpr std::array<Translation, 3> SHOPKEEPER_COUNTER_LINES{
    Translation{
        .english =
            "MY ABACUS, MY TEA. YOU CAN TUNE THE RADIO. IS ALREADY A LOT.",
        .french =
            "MON BOULIER, MON THÉ. TOI POUVOIR RÉGLER RADIO. DÉJÀ BEAUCOUP."},
    Translation{
        .english = "AGAIN? THE RADIO IS FOR YOU, TOVARISCH. NOT MY THINGS.",
        .french = "ENCORE ? RADIO ÊTRE POUR TOI, TOVARISCH. PAS MES AFFAIRES."},
    Translation{
        .english = "FINE. TOUCH EVERYTHING. I AM TOO OLD FOR THIS.",
        .french = "BON. TOUCHE TOUT. MOI TROP VIEUX POUR CES BÊTISES."}};

/** @brief A click on his bottles, in turn. */
constexpr std::array<Translation, 4> SHOPKEEPER_BOTTLES_LINES{
    Translation{.english = "HANDS OFF! IS NOT FOR PILOTS. IS FOR ME.",
                .french = "PAS TOUCHER ! PAS POUR PILOTES. POUR MOI."},
    Translation{.english = "THIS ONE IS MEDICINE. VERY STRONG MEDICINE.",
                .french = "CELLE-LÀ ÊTRE MÉDICAMENT. TRÈS FORT MÉDICAMENT."},
    Translation{
        .english = "NO TOUCH. LAST ONE WHO TOUCHED IS NOW IN SIBERIA.",
        .french =
            "PAS TOUCHER. DERNIER QUI TOUCHER ÊTRE MAINTENANT EN SIBÉRIE."},
    Translation{
        .english = "BOTTLES ARE NOT FOR SALE. BOTTLES ARE FOR WINTER.",
        .french = "BOUTEILLES PAS À VENDRE. BOUTEILLES ÊTRE POUR HIVER."}};

/** @brief The radio tuned to the theme of Papers, Please, in turn, and back
 * to the music of the shop. */
constexpr std::array<Translation, 4> SHOPKEEPER_BORDER_RADIO_LINES{
    Translation{
        .english =
            "GLORY TO ARSTOTZKA! PAPERS, PLEASE. NO PAPERS? KOPECKS, PLEASE.",
        .french =
            "GLOIRE À ARSTOTZKA ! VOS PAPIERS. PAS PAPIERS ? VOS KOPECKS."},
    Translation{.english = "AH, BORDER MUSIC. CAUSE NO TROUBLE IN MY SHOP.",
                .french = "AH, MUSIQUE DE FRONTIÈRE. PAS FAIRE HISTOIRES ICI."},
    Translation{
        .english = "YOUR PAPERS ARE IN ORDER. YOUR WALLET, WE WILL SEE.",
        .french = "TES PAPIERS ÊTRE EN RÈGLE. TON PORTEFEUILLE, ON VERRA."},
    Translation{.english = "ENTRY DENIED... JOKE! COME IN, BUY HAT.",
                .french = "ENTRÉE REFUSÉE... BLAGUE ! ENTRE, ACHÈTE CHAPEAU."}};
/** @brief The radio tuned to Kavorka, a happy acoustic music, busy and
 * frantic. */
constexpr Translation SHOPKEEPER_DANCE_RADIO_LINE{
    .english = "SO HAPPY, SO FAST! MY TENTACLES CANNOT KEEP UP.",
    .french = "SI JOYEUX, SI RAPIDE ! MES TENTACULES PAS SUIVRE."};
constexpr Translation SHOPKEEPER_SHOP_MUSIC_LINE{
    .english = "AH, BACK TO MOSCOW. MUCH BETTER.",
    .french = "AH, RETOUR À MOSCOU. BEAUCOUP MIEUX."};

}  // namespace rtype::client
