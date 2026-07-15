#!/usr/bin/env bash
# build.sh
# Recompile le moteur IEEE 754 en WebAssembly (web/dist/ieee754.js).
# Necessite Emscripten installe et active (emsdk).
# Voir : https://emscripten.org/docs/getting_started/downloads.html

set -e

em++ -std=c++17 -O3 \
  src/ieee754_core.cpp web/web.cpp \
  -I src \
  -s WASM=1 -s SINGLE_FILE=1 -s MODULARIZE=1 -s EXPORT_NAME='IEEE754Module' \
  -s EXPORTED_FUNCTIONS='["_decimalVersBinaireJSON","_binaireVersDecimalJSON"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
  -s ENVIRONMENT='web' \
  -s ALLOW_MEMORY_GROWTH=0 \
  --no-entry \
  -o web/dist/ieee754.js

echo "OK : web/dist/ieee754.js genere."
