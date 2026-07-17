# Norme-IEEE

Convertisseur IEEE 754 simple précision (32 bits) — décimal ↔ binaire.

## Structure du projet

```
convertit.cpp        # version console originale (inchangée)
convertit.exe         # exécutable original (inchangé)

src/
  ieee754_core.h/.cpp # logique pure de conversion (aucune I/O), utilisée
                       # par la version console ET la version web
  main.cpp             # version console (comportement identique à convertit.cpp)

web/
  index.html           # page du convertisseur en ligne
  web.cpp               # pont C++ -> JavaScript (exposé via Emscripten)
  dist/ieee754.js       # module WebAssembly compilé (généré par build.sh)

.github/workflows/deploy.yml  # compile web.cpp en WASM et déploie sur GitHub Pages à chaque push sur main
build.sh              # recompile le module WASM localement
```

## Pourquoi cette séparation ?

Le programme console original lit son entrée via `cin` dans une boucle
bloquante — impossible à exécuter tel quel dans un navigateur. La logique de
conversion (validation, décomposition des bits, cas spéciaux) a donc été
extraite dans `src/ieee754_core.cpp`, sans aucune E/S, pour être réutilisée
à l'identique par :
- `src/main.cpp` — la version console (même comportement qu'avant)
- `web/web.cpp` — le pont vers JavaScript, compilé en WebAssembly

## Déploiement

Le site se déploie automatiquement sur GitHub Pages à chaque `git push` sur
`main`, via `.github/workflows/deploy.yml` (compilation Emscripten incluse).

**Pour l'activer une seule fois :** Settings → Pages → Source →
`GitHub Actions`.

## Compiler soi-même

**Version console (g++) :**
```bash
g++ -std=c++17 -O2 src/main.cpp src/ieee754_core.cpp -o convertit
```

**Version web (Emscripten requis) :**
```bash
./build.sh
```
https://dakin-blair.github.io/Norme-IEEE/
