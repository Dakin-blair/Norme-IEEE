/*
 * =============================================================
 *  Convertisseur IEEE 754 - Simple précision (32 bits)
 *  Décimal <-> Binaire
 * =============================================================
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>
#include <cstring>
#include <algorithm>
#include <limits>
#include <cstdint>

using namespace std;

// ============================================================
//  FONCTION : Retirer les espaces d'une chaîne de caractères
// ============================================================
string retirerEspaces(const string& chaine) {
    string chaineSansEspaces;
    for (char caractere : chaine) {
        if (caractere != ' ') {
            chaineSansEspaces += caractere;
        }
    }
    return chaineSansEspaces;
}

// ============================================================
//  FONCTION : Vérifier si une chaîne est un binaire valide
//             (contient uniquement des 0 et des 1)
// ============================================================
bool estBinaireValide(const string& chaine) {
    if (chaine.empty()) return false;
    for (char caractere : chaine) {
        if (caractere != '0' && caractere != '1') {
            return false;
        }
    }
    return true;
}

// ============================================================
//  FONCTION : Vérifier si une chaîne est un nombre décimal valide
//             (entier, flottant, négatif, cas spéciaux inf/nan)
// ============================================================
bool estNombreValide(const string& chaine) {
    if (chaine.empty()) return false;

    // Convertir en minuscules pour comparer les cas spéciaux
    string chaineMinuscule = chaine;
    transform(chaineMinuscule.begin(), chaineMinuscule.end(), chaineMinuscule.begin(), ::tolower);

    // Cas spéciaux acceptés
    if (chaineMinuscule == "inf"  || chaineMinuscule == "+inf" ||
        chaineMinuscule == "-inf" || chaineMinuscule == "nan") {
        return true;
    }

    // Vérification caractère par caractère
    int nombreDePoints = 0;
    int nombreDeSignes = 0;

    for (size_t i = 0; i < chaine.size(); i++) {
        char caractere = chaine[i];

        if (caractere == '+' || caractere == '-') {
            // Le signe est autorisé uniquement en première position
            if (i != 0) return false;
            nombreDeSignes++;
        }
        else if (caractere == '.' || caractere == ',') {
            // Un seul point (ou virgule) est autorisé
            nombreDePoints++;
            if (nombreDePoints > 1) return false;
        }
        else if (!isdigit(caractere)) {
            // Tout autre caractère est invalide
            return false;
        }
    }

    // Rejeter les entrées comme "+", "-", ".", ","
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

    return true;
}

// ============================================================
//  FONCTION : Conversion Décimal -> Binaire IEEE 754 (32 bits)
// ============================================================
void decimalVersBinaire() {
    string entreeUtilisateur;

    cout << endl;
    cout << "===============================================" << endl;
    cout << "   Conversion : Decimal -> Binaire IEEE 754"   << endl;
    cout << "===============================================" << endl;

    // Boucle : redemander tant que l'entrée est invalide
    while (true) {
        cout << "Entrez un nombre decimal (ex: 45.026, -3.14, 0, inf, nan) : ";
        cin >> entreeUtilisateur;

        // Remplacer la virgule par un point si l'utilisateur utilise une virgule
        replace(entreeUtilisateur.begin(), entreeUtilisateur.end(), ',', '.');

        // Vérification de la validité de l'entrée
        if (estNombreValide(entreeUtilisateur)) {
            break; // Entrée valide, on sort de la boucle
        }

        cout << endl;
        cout << "  ERREUR : \"" << entreeUtilisateur << "\" n'est pas un nombre valide." << endl;
        cout << "  Veuillez entrer un nombre (ex: 45.026, -3, 0, inf, nan)."            << endl;
        cout << endl;
    }

    // Convertir la chaîne en nombre flottant
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

    // Copier les bits du float dans un entier non signé de 32 bits
    uint32_t bits32;
    memcpy(&bits32, &nombreFlottant, sizeof(bits32));

    // Extraire les trois champs IEEE 754
    uint32_t bitSigne     = (bits32 >> 31) & 0x1;      // 1 bit
    uint32_t bitsExposant = (bits32 >> 23) & 0xFF;     // 8 bits
    uint32_t bitsMantisse =  bits32 & 0x7FFFFF;        // 23 bits

    int exposantReel = (int)bitsExposant - 127;        // Exposant sans le biais

    // Affichage des résultats
    cout << endl;
    cout << "  Nombre entre       : " << entreeUtilisateur                      << endl;
    cout << endl;
    cout << "  Decomposition IEEE 754 (32 bits) :"                              << endl;
    cout << "  -----------------------------------------------"                 << endl;
    cout << "  Signe     (1 bit)  : " << bitSigne                               << endl;
    cout << "  Exposant  (8 bits) : " << bitset<8>(bitsExposant)
         << "  (valeur brute = "      << bitsExposant
         << ", exposant reel = "      << exposantReel << ")"                    << endl;
    cout << "  Mantisse  (23 bits): " << bitset<23>(bitsMantisse)               << endl;
    cout << "  -----------------------------------------------"                 << endl;
    cout << "  Representation complete (32 bits) :"                             << endl;
    cout << "  " << bitSigne << " "
         << bitset<8>(bitsExposant) << " "
         << bitset<23>(bitsMantisse)                                            << endl;

    // Détection des cas spéciaux
    cout << endl;
    cout << "  Cas special        : ";
    if (bitsExposant == 0xFF && bitsMantisse == 0) {
        cout << (bitSigne ? "Infini negatif (-inf)" : "Infini positif (+inf)") << endl;
    } else if (bitsExposant == 0xFF && bitsMantisse != 0) {
        cout << "NaN (Not a Number)"                                            << endl;
    } else if (bitsExposant == 0 && bitsMantisse == 0) {
        cout << (bitSigne ? "Zero negatif (-0)" : "Zero positif (+0)")         << endl;
    } else if (bitsExposant == 0) {
        cout << "Nombre denormalise"                                            << endl;
    } else {
        cout << "Aucun (nombre normalise)"                                      << endl;
    }
}

// ============================================================
//  FONCTION : Conversion Binaire IEEE 754 (32 bits) -> Décimal
// ============================================================
void binaireVersDecimal() {
    string entreeUtilisateur;

    cout << endl;
    cout << "===============================================" << endl;
    cout << "   Conversion : Binaire IEEE 754 -> Decimal"   << endl;
    cout << "===============================================" << endl;
    cout << "Entrez 32 bits IEEE 754 (espaces autorises)"   << endl;
    cout << "Exemple : 0 10000010 01101000000110001001011"  << endl;
    cout << "Votre entree : ";
    cin.ignore();
    getline(cin, entreeUtilisateur);

    // Retirer les espaces
    string chaineBinaire = retirerEspaces(entreeUtilisateur);

    // Boucle : redemander tant que l'entrée contient des caractères invalides
    while (!estBinaireValide(chaineBinaire)) {
        cout << endl;
        cout << "  ERREUR : l'entree contient des caracteres invalides." << endl;
        cout << "  Veuillez entrer uniquement des 0 et des 1."          << endl;
        cout << endl;
        cout << "Votre entree : ";
        getline(cin, entreeUtilisateur);
        chaineBinaire = retirerEspaces(entreeUtilisateur);
    }

    // Si moins de 32 bits : compléter avec des 0 à droite
    if (chaineBinaire.size() < 32) {
        int nombreDeZerosAjoutes = 32 - chaineBinaire.size();
        cout << endl;
        cout << "  INFO : seulement " << chaineBinaire.size()
             << " bit(s) entres. Les " << nombreDeZerosAjoutes
             << " bit(s) manquants sont completes par des 0 a droite."  << endl;
        chaineBinaire = chaineBinaire + string(nombreDeZerosAjoutes, '0');
    }

    // Si plus de 32 bits : erreur et redemander
    while (chaineBinaire.size() > 32) {
        cout << endl;
        cout << "  ERREUR : vous avez entre " << chaineBinaire.size()
             << " bit(s). Le maximum autorise est 32 bits."             << endl;
        cout << endl;
        cout << "Votre entree : ";
        getline(cin, entreeUtilisateur);
        chaineBinaire = retirerEspaces(entreeUtilisateur);

        // Revalider
        while (!estBinaireValide(chaineBinaire)) {
            cout << endl;
            cout << "  ERREUR : l'entree contient des caracteres invalides." << endl;
            cout << "  Veuillez entrer uniquement des 0 et des 1."          << endl;
            cout << endl;
            cout << "Votre entree : ";
            getline(cin, entreeUtilisateur);
            chaineBinaire = retirerEspaces(entreeUtilisateur);
        }

        // Recompléter si nécessaire
        if (chaineBinaire.size() < 32) {
            int nombreDeZerosAjoutes = 32 - chaineBinaire.size();
            cout << endl;
            cout << "  INFO : seulement " << chaineBinaire.size()
                 << " bit(s) entres. Les " << nombreDeZerosAjoutes
                 << " bit(s) manquants sont completes par des 0 a droite." << endl;
            chaineBinaire = chaineBinaire + string(nombreDeZerosAjoutes, '0');
        }
    }

    // Reconstruire le nombre 32 bits depuis la chaîne
    uint32_t bits32 = 0;
    for (int i = 0; i < 32; i++) {
        bits32 = (bits32 << 1) | (chaineBinaire[i] - '0');
    }

    // Extraire les trois champs IEEE 754
    uint32_t bitSigne     = (bits32 >> 31) & 0x1;
    uint32_t bitsExposant = (bits32 >> 23) & 0xFF;
    uint32_t bitsMantisse =  bits32 & 0x7FFFFF;

    int exposantReel = (int)bitsExposant - 127;

    // Reconvertir les bits en nombre flottant
    float nombreFlottant;
    memcpy(&nombreFlottant, &bits32, sizeof(nombreFlottant));

    // Affichage de la décomposition
    cout << endl;
    cout << "  Decomposition IEEE 754 :"                                        << endl;
    cout << "  -----------------------------------------------"                 << endl;
    cout << "  Signe     (1 bit)  : " << bitSigne                               << endl;
    cout << "  Exposant  (8 bits) : " << bitset<8>(bitsExposant)
         << "  (valeur brute = "      << bitsExposant
         << ", exposant reel = "      << exposantReel << ")"                    << endl;
    cout << "  Mantisse  (23 bits): " << bitset<23>(bitsMantisse)               << endl;
    cout << "  -----------------------------------------------"                 << endl;

    // Affichage du résultat selon le cas
    cout << "  Valeur decimale    : ";
    if (bitsExposant == 0xFF && bitsMantisse == 0) {
        cout << (bitSigne ? "-Infini" : "+Infini")                                      << endl;
        cout << "  Cas special        : Infini " << (bitSigne ? "negatif" : "positif") << endl;
    } else if (bitsExposant == 0xFF && bitsMantisse != 0) {
        cout << "NaN (Not a Number)"                                                    << endl;
        cout << "  Cas special        : NaN"                                            << endl;
    } else if (bitsExposant == 0 && bitsMantisse == 0) {
        cout << (bitSigne ? "-0" : "+0")                                                << endl;
        cout << "  Cas special        : Zero " << (bitSigne ? "negatif" : "positif")   << endl;
    } else if (bitsExposant == 0) {
        cout << nombreFlottant                                                          << endl;
        cout << "  Cas special        : Nombre denormalise"                             << endl;
    } else {
        cout << nombreFlottant                                                          << endl;
        cout << "  Cas special        : Aucun (nombre normalise)"                       << endl;
    }
}

// ============================================================
//  FONCTION PRINCIPALE : Menu interactif
// ============================================================
int main() {
    int choixUtilisateur;

    cout << "=================================================" << endl;
    cout << "   Convertisseur IEEE 754 - Simple precision"     << endl;
    cout << "=================================================" << endl;
    cout << endl;
    cout << "  Choisissez une option :"                             << endl;
    cout << "  1 - Convertir un nombre decimal en binaire IEEE 754" << endl;
    cout << "  2 - Convertir un binaire IEEE 754 en nombre decimal" << endl;
    cout << "  3 - Quitter"                                         << endl;

    // Boucle : redemander tant que le choix est invalide
    while (true) {
        cout << endl;
        cout << "  Votre choix : ";
        cin >> choixUtilisateur;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << endl;
            cout << "  ERREUR : veuillez entrer 1, 2 ou 3." << endl;
            continue;
        }

        if (choixUtilisateur >= 1 && choixUtilisateur <= 3) {
            break;
        }

        cout << endl;
        cout << "  ERREUR : choix invalide. Veuillez entrer 1, 2 ou 3." << endl;
    }

    switch (choixUtilisateur) {
        case 1:
            decimalVersBinaire();
            break;
        case 2:
            binaireVersDecimal();
            break;
        case 3:
            cout << endl;
            cout << "  Au revoir !" << endl;
            break;
    }

    return 0;
}