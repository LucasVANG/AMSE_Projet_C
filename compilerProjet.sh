#!/bin/bash
# -*- ENCODING: UTF-8 -*-
echo "[COMPILER SCRIPT] ######### Programme de compilation du projet AMSE_C #########"
echo "[COMPILER SCRIPT] Processus de compilation démarré"

if [ -d "bin" ]
then
else
	echo "[COMPILER SCRIPT] Dossier 'bin' pas trouvé, création fichier 'bin'."
    mkdir bin
fi

if [ -d "build" ]; then
    cd build
else
    echo "[COMPILER SCRIPT] Création dossier build pour Cmake"
    mkdir build
    cd build
fi
echo "[COMPILER SCRIPT] Configuration des fichiers avec Cmake"
cmake ..
echo "[COMPILER SCRIPT] Compilation des fichiers Cmake"
make
echo "[COMPILER SCRIPT] Processus de compilation terminé, les fichiers binaires sont dans le dossier 'bin'."
exit