/*  ieee754_core.h
 *  Logique pure de conversion IEEE 754 simple precision (32 bits).
 *  Ne fait AUCUNE entree/sortie (pas de cin/cout) : reutilisable telle
 *  quelle par la version console (main.cpp) et par la version web (web.cpp).
 */

#ifndef IEEE754_CORE_H
#define IEEE754_CORE_H

#include <string>
#include <cstdint>

//  Retire tous les espaces d'une chaine.
std::string retirerEspaces(const std::string& chaine);

//  Verifie qu'une chaine ne contient que des '0' et des '1'.
bool estBinaireValide(const std::string& chaine);

//  Verifie qu'une chaine represente un nombre decimal valide
//  (entier, flottant, negatif, ou un des cas speciaux inf/-inf/nan).
bool estNombreValide(const std::string& chaine);

//  Resultat d'une conversion Decimal -> Binaire.
struct ResultatDecimalVersBinaire {
    bool        valide;          // false si l'entree n'etait pas un nombre valide
    std::string messageErreur;   // message explicatif si valide == false
    std::string entreeNettoyee;  // entree apres normalisation (virgule -> point)
    uint32_t    bits;            // representation brute 32 bits
    int         signe;           // 0 ou 1
    int         exposantBrut;    // valeur brute de l'exposant (0-255)
    int         exposantReel;    // exposant brut - 127
    int         mantisse;        // 23 bits de mantisse
    std::string casSpecial;      // "Aucun (nombre normalise)", "NaN", etc.
};

//  Convertit une chaine decimale (ex: "45.026", "-3.14", "inf", "nan")
//  en sa representation IEEE 754 32 bits, avec decomposition des champs.
ResultatDecimalVersBinaire convertirDecimalVersBinaire(const std::string& entreeUtilisateur);

//  Resultat d'une conversion Binaire -> Decimal.
struct ResultatBinaireVersDecimal {
    bool        valide;
    std::string messageErreur;
    std::string binaireNettoye;  // chaine sans espaces, completee a 32 bits
    int         nombreBitsSaisis;// nombre de bits reellement saisis avant completion
    uint32_t    bits;
    int         signe;
    int         exposantBrut;
    int         exposantReel;
    int         mantisse;
    float       valeur;          // valeur decimale reconstruite
    std::string casSpecial;
};

//  Convertit une chaine binaire (espaces tolerees, jusqu'a 32 bits, completee
//  a droite avec des 0 si plus courte) en sa valeur decimale IEEE 754.
ResultatBinaireVersDecimal convertirBinaireVersDecimal(const std::string& entreeUtilisateur);

#endif // IEEE754_CORE_H
