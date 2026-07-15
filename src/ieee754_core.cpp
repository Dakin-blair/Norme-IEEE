/*  ieee754_core.cpp
 *  Implementation de la logique pure de conversion IEEE 754.
 *  Extrait et adapte de convertit.cpp (version console originale) :
 *  meme logique de validation et de decomposition des bits, mais sans
 *  aucune interaction cin/cout, pour etre appelable depuis la console
 *  ET depuis le module WebAssembly.
 */

#include "ieee754_core.h"
#include <sstream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <limits>

using namespace std;

string retirerEspaces(const string& chaine) {
    string chaineSansEspaces;
    for (char caractere : chaine) {
        if (caractere != ' ') {
            chaineSansEspaces += caractere;
        }
    }
    return chaineSansEspaces;
}

bool estBinaireValide(const string& chaine) {
    if (chaine.empty()) return false;
    for (char caractere : chaine) {
        if (caractere != '0' && caractere != '1') {
            return false;
        }
    }
    return true;
}

bool estNombreValide(const string& chaine) {
    if (chaine.empty()) return false;

    string chaineMinuscule = chaine;
    transform(chaineMinuscule.begin(), chaineMinuscule.end(), chaineMinuscule.begin(), ::tolower);

    if (chaineMinuscule == "inf"  || chaineMinuscule == "+inf" ||
        chaineMinuscule == "-inf" || chaineMinuscule == "nan") {
        return true;
    }

    int nombreDePoints = 0;

    for (size_t i = 0; i < chaine.size(); i++) {
        char caractere = chaine[i];

        if (caractere == '+' || caractere == '-') {
            if (i != 0) return false;
        }
        else if (caractere == '.' || caractere == ',') {
            nombreDePoints++;
            if (nombreDePoints > 1) return false;
        }
        else if (!isdigit(static_cast<unsigned char>(caractere))) {
            return false;
        }
    }

    string chaineSansSignes = chaine;
    if (!chaineSansSignes.empty() &&
        (chaineSansSignes[0] == '+' || chaineSansSignes[0] == '-')) {
        chaineSansSignes = chaineSansSignes.substr(1);
    }
    if (chaineSansSignes.empty() ||
        chaineSansSignes == "."  ||
        chaineSansSignes == ",") {
        return false;
    }

    if (chaineSansSignes.size() >= 2 &&
        chaineSansSignes[0] == '0'   &&
        isdigit(static_cast<unsigned char>(chaineSansSignes[1]))) {
        return false;
    }

    return true;
}

static string determinerCasSpecial(uint32_t bitSigne, uint32_t bitsExposant, uint32_t bitsMantisse) {
    if (bitsExposant == 0xFF && bitsMantisse == 0) {
        return bitSigne ? "Infini negatif (-inf)" : "Infini positif (+inf)";
    } else if (bitsExposant == 0xFF && bitsMantisse != 0) {
        return "NaN (Not a Number)";
    } else if (bitsExposant == 0 && bitsMantisse == 0) {
        return bitSigne ? "Zero negatif (-0)" : "Zero positif (+0)";
    } else if (bitsExposant == 0) {
        return "Nombre denormalise";
    }
    return "Aucun (nombre normalise)";
}

ResultatDecimalVersBinaire convertirDecimalVersBinaire(const string& entreeUtilisateurBrute) {
    ResultatDecimalVersBinaire resultat{};

    string entreeUtilisateur = entreeUtilisateurBrute;
    replace(entreeUtilisateur.begin(), entreeUtilisateur.end(), ',', '.');

    if (!estNombreValide(entreeUtilisateur)) {
        resultat.valide = false;
        resultat.messageErreur = "\"" + entreeUtilisateurBrute + "\" n'est pas un nombre decimal valide.";
        return resultat;
    }

    resultat.entreeNettoyee = entreeUtilisateur;

    float nombreFlottant;
    string chaineMinuscule = entreeUtilisateur;
    transform(chaineMinuscule.begin(), chaineMinuscule.end(), chaineMinuscule.begin(), ::tolower);

    if (chaineMinuscule == "nan") {
        nombreFlottant = numeric_limits<float>::quiet_NaN();
    } else if (chaineMinuscule == "inf" || chaineMinuscule == "+inf") {
        nombreFlottant = numeric_limits<float>::infinity();
    } else if (chaineMinuscule == "-inf") {
        nombreFlottant = -numeric_limits<float>::infinity();
    } else {
        istringstream convertisseur(entreeUtilisateur);
        convertisseur >> nombreFlottant;
    }

    uint32_t bits32;
    memcpy(&bits32, &nombreFlottant, sizeof(bits32));

    uint32_t bitSigne     = (bits32 >> 31) & 0x1;
    uint32_t bitsExposant = (bits32 >> 23) & 0xFF;
    uint32_t bitsMantisse =  bits32 & 0x7FFFFF;

    resultat.valide        = true;
    resultat.bits          = bits32;
    resultat.signe          = (int)bitSigne;
    resultat.exposantBrut   = (int)bitsExposant;
    resultat.exposantReel   = (int)bitsExposant - 127;
    resultat.mantisse       = (int)bitsMantisse;
    resultat.casSpecial     = determinerCasSpecial(bitSigne, bitsExposant, bitsMantisse);

    return resultat;
}

ResultatBinaireVersDecimal convertirBinaireVersDecimal(const string& entreeUtilisateurBrute) {
    ResultatBinaireVersDecimal resultat{};

    string chaineBinaire = retirerEspaces(entreeUtilisateurBrute);

    if (!estBinaireValide(chaineBinaire)) {
        resultat.valide = false;
        resultat.messageErreur = "L'entree contient des caracteres invalides. Veuillez entrer uniquement des 0 et des 1.";
        return resultat;
    }

    if (chaineBinaire.size() > 32) {
        resultat.valide = false;
        resultat.messageErreur = "Vous avez entre " + to_string(chaineBinaire.size()) +
                                  " bit(s). Le maximum autorise est 32 bits.";
        return resultat;
    }

    resultat.nombreBitsSaisis = (int)chaineBinaire.size();

    if (chaineBinaire.size() < 32) {
        int nombreDeZerosAjoutes = 32 - (int)chaineBinaire.size();
        chaineBinaire = chaineBinaire + string(nombreDeZerosAjoutes, '0');
    }

    resultat.binaireNettoye = chaineBinaire;

    uint32_t bits32 = 0;
    for (int i = 0; i < 32; i++) {
        bits32 = (bits32 << 1) | (uint32_t)(chaineBinaire[i] - '0');
    }

    uint32_t bitSigne     = (bits32 >> 31) & 0x1;
    uint32_t bitsExposant = (bits32 >> 23) & 0xFF;
    uint32_t bitsMantisse =  bits32 & 0x7FFFFF;

    float nombreFlottant;
    memcpy(&nombreFlottant, &bits32, sizeof(nombreFlottant));

    resultat.valide        = true;
    resultat.bits          = bits32;
    resultat.signe          = (int)bitSigne;
    resultat.exposantBrut   = (int)bitsExposant;
    resultat.exposantReel   = (int)bitsExposant - 127;
    resultat.mantisse       = (int)bitsMantisse;
    resultat.valeur         = nombreFlottant;
    resultat.casSpecial     = determinerCasSpecial(bitSigne, bitsExposant, bitsMantisse);

    return resultat;
}
