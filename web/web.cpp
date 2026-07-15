/*  web.cpp
 *  Pont entre la logique C++ (ieee754_core) et JavaScript, compile en
 *  WebAssembly via Emscripten. Expose deux fonctions qui prennent une
 *  chaine C en entree et retournent un JSON (chaine C) que le JS parse.
 *
 *  Aucune logique de conversion ici : tout vient de ieee754_core.cpp,
 *  exactement la meme que celle utilisee par la version console.
 */

#include <emscripten/emscripten.h>
#include <sstream>
#include <string>
#include <bitset>
#include <cmath>
#include "../src/ieee754_core.h"

using namespace std;

// Echappe les guillemets et antislashs pour produire un JSON valide.
static string jsonEchapper(const string& s) {
    string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

static string floatVersJSON(float v) {
    if (std::isnan(v)) return "\"NaN\"";
    if (std::isinf(v)) return v > 0 ? "\"Infinity\"" : "\"-Infinity\"";
    ostringstream oss;
    oss << v;
    return oss.str();
}

extern "C" {

// Buffer statique conserve entre deux appels : suffisant car le JS lit
// la chaine retournee de maniere synchrone, immediatement apres l'appel.
static string g_bufferReponse;

EMSCRIPTEN_KEEPALIVE
const char* decimalVersBinaireJSON(const char* entreeC) {
    string entree(entreeC ? entreeC : "");
    ResultatDecimalVersBinaire r = convertirDecimalVersBinaire(entree);

    ostringstream json;
    json << "{";
    json << "\"valide\":" << (r.valide ? "true" : "false");
    if (!r.valide) {
        json << ",\"erreur\":\"" << jsonEchapper(r.messageErreur) << "\"";
    } else {
        json << ",\"entree\":\"" << jsonEchapper(r.entreeNettoyee) << "\"";
        json << ",\"bits\":\"" << bitset<32>(r.bits).to_string() << "\"";
        json << ",\"signe\":" << r.signe;
        json << ",\"exposantBrut\":" << r.exposantBrut;
        json << ",\"exposantBits\":\"" << bitset<8>(r.exposantBrut).to_string() << "\"";
        json << ",\"exposantReel\":" << r.exposantReel;
        json << ",\"mantisse\":" << r.mantisse;
        json << ",\"mantisseBits\":\"" << bitset<23>(r.mantisse).to_string() << "\"";
        json << ",\"casSpecial\":\"" << jsonEchapper(r.casSpecial) << "\"";
    }
    json << "}";

    g_bufferReponse = json.str();
    return g_bufferReponse.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* binaireVersDecimalJSON(const char* entreeC) {
    string entree(entreeC ? entreeC : "");
    ResultatBinaireVersDecimal r = convertirBinaireVersDecimal(entree);

    ostringstream json;
    json << "{";
    json << "\"valide\":" << (r.valide ? "true" : "false");
    if (!r.valide) {
        json << ",\"erreur\":\"" << jsonEchapper(r.messageErreur) << "\"";
    } else {
        json << ",\"bits\":\"" << bitset<32>(r.bits).to_string() << "\"";
        json << ",\"nombreBitsSaisis\":" << r.nombreBitsSaisis;
        json << ",\"signe\":" << r.signe;
        json << ",\"exposantBrut\":" << r.exposantBrut;
        json << ",\"exposantBits\":\"" << bitset<8>(r.exposantBrut).to_string() << "\"";
        json << ",\"exposantReel\":" << r.exposantReel;
        json << ",\"mantisse\":" << r.mantisse;
        json << ",\"mantisseBits\":\"" << bitset<23>(r.mantisse).to_string() << "\"";
        json << ",\"valeur\":" << floatVersJSON(r.valeur);
        json << ",\"casSpecial\":\"" << jsonEchapper(r.casSpecial) << "\"";
    }
    json << "}";

    g_bufferReponse = json.str();
    return g_bufferReponse.c_str();
}

} // extern "C"
