/*  main.cpp
 *  Convertisseur IEEE 754 - Simple precision (32 bits) - Version console.
 *  Decimal <-> Binaire
 *
 *  Toute la logique de conversion vit dans ieee754_core.{h,cpp} : ce fichier
 *  ne gere que l'interaction avec l'utilisateur (cin/cout) et l'affichage.
 */

#include <iostream>
#include <bitset>
#include <iomanip>
#include "ieee754_core.h"

using namespace std;

void decimalVersBinaire() {
    string entreeUtilisateur;

    cout << endl;
    cout << " " << endl;
    cout << "   Conversion : Decimal -> Binaire IEEE 754" << endl;
    cout << " " << endl;

    ResultatDecimalVersBinaire resultat;
    while (true) {
        cout << "Entrez un nombre decimal (ex: 45.026, -3.14, 0, inf, nan) : ";
        cin >> entreeUtilisateur;

        resultat = convertirDecimalVersBinaire(entreeUtilisateur);
        if (resultat.valide) break;

        cout << endl;
        cout << "  ERREUR : " << resultat.messageErreur << endl;
        cout << "  Veuillez entrer un nombre (ex: 45.026, -3, 0, inf, nan)." << endl;
        cout << endl;
    }

    cout << endl;
    cout << "  Nombre entre       : " << resultat.entreeNettoyee << endl;
    cout << endl;
    cout << "  Decomposition IEEE 754 (32 bits) :" << endl;
    cout << "-" << endl;
    cout << "  Signe     (1 bit)  : " << resultat.signe << endl;
    cout << "  Exposant  (8 bits) : " << bitset<8>(resultat.exposantBrut)
         << "  (valeur brute = " << resultat.exposantBrut
         << ", exposant reel = " << resultat.exposantReel << ")" << endl;
    cout << "  Mantisse  (23 bits): " << bitset<23>(resultat.mantisse) << endl;
    cout << "-" << endl;
    cout << "  Representation complete (32 bits) :" << endl;
    cout << "  " << resultat.signe << " "
         << bitset<8>(resultat.exposantBrut) << " "
         << bitset<23>(resultat.mantisse) << endl;
    cout << endl;
    cout << "  Cas special        : " << resultat.casSpecial << endl;
}

void binaireVersDecimal() {
    string entreeUtilisateur;

    cout << endl;
    cout << "-" << endl;
    cout << "   Conversion : Binaire IEEE 754 -> Decimal" << endl;
    cout << "-" << endl;
    cout << "Entrez 32 bits IEEE 754 (espaces autorises)" << endl;
    cout << "Exemple : 0 10000010 01101000000110001001011" << endl;
    cout << "Votre entree : ";
    cin.ignore();
    getline(cin, entreeUtilisateur);

    ResultatBinaireVersDecimal resultat = convertirBinaireVersDecimal(entreeUtilisateur);

    while (!resultat.valide) {
        cout << endl;
        cout << "  ERREUR : " << resultat.messageErreur << endl;
        cout << endl;
        cout << "Votre entree : ";
        getline(cin, entreeUtilisateur);
        resultat = convertirBinaireVersDecimal(entreeUtilisateur);
    }

    if (resultat.nombreBitsSaisis < 32) {
        cout << endl;
        cout << "  INFO : seulement " << resultat.nombreBitsSaisis
             << " bit(s) entres. Les " << (32 - resultat.nombreBitsSaisis)
             << " bit(s) manquants sont completes par des 0 a droite." << endl;
    }

    cout << endl;
    cout << "  Decomposition IEEE 754 :" << endl;
    cout << "-" << endl;
    cout << "  Signe     (1 bit)  : " << resultat.signe << endl;
    cout << "  Exposant  (8 bits) : " << bitset<8>(resultat.exposantBrut)
         << "  (valeur brute = " << resultat.exposantBrut
         << ", exposant reel = " << resultat.exposantReel << ")" << endl;
    cout << "  Mantisse  (23 bits): " << bitset<23>(resultat.mantisse) << endl;
    cout << "-" << endl;

    cout << "  Valeur decimale    : ";
    if (resultat.casSpecial.rfind("Infini", 0) == 0) {
        cout << (resultat.signe ? "-Infini" : "+Infini") << endl;
    } else if (resultat.casSpecial.rfind("NaN", 0) == 0) {
        cout << "NaN (Not a Number)" << endl;
    } else if (resultat.casSpecial.rfind("Zero", 0) == 0) {
        cout << (resultat.signe ? "-0" : "+0") << endl;
    } else {
        cout << fixed << setprecision(6) << resultat.valeur << endl;
    }
    cout << "  Cas special        : " << resultat.casSpecial << endl;
}

int main() {
    int choixUtilisateur;

    cout << "-" << endl;
    cout << "   Convertisseur IEEE 754 - Simple precision" << endl;
    cout << "-" << endl;
    cout << endl;
    cout << "  Choisissez une option :" << endl;
    cout << "  1 - Convertir un nombre decimal en binaire IEEE 754" << endl;
    cout << "  2 - Convertir un binaire IEEE 754 en nombre decimal" << endl;
    cout << "  0 - Quitter" << endl;

    while (true) {
        cout << endl;
        cout << "  Votre choix : ";
        cin >> choixUtilisateur;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << endl;
            cout << "  ERREUR : veuillez entrer 0, 1 ou 2." << endl;
            continue;
        }

        if (choixUtilisateur >= 0 && choixUtilisateur <= 2) {
            break;
        }

        cout << endl;
        cout << "  ERREUR : choix invalide. Veuillez entrer 0, 1 ou 2." << endl;
    }

    switch (choixUtilisateur) {
        case 1:
            decimalVersBinaire();
            break;
        case 2:
            binaireVersDecimal();
            break;
        case 0:
            cout << endl;
            cout << "  Au revoir !" << endl;
            break;
    }

    return 0;
}
