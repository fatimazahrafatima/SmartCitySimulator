#ifndef ENGINE_H
#define ENGINE_H

// On inclut les fichiers dont on a besoin ici
#include "config.h"
#include "vehicle.h"

// --- VARIABLES PARTAGÉES (GLOBALES) ---
// Le mot "extern" dit au programme : "Ces variables existent déjà dans un autre fichier (world.cpp),
// mais on veut pouvoir les lire et les modifier ici aussi."

extern GameState currentState; // Permet de savoir si on est dans le MENU ou dans le JEU
extern bool isNight;           // Permet de savoir si c'est la nuit (pour allumer les phares)

// --- FONCTIONS UTILITAIRES ---
// Outils pour gérer l'interface graphique

// Dessine un bouton rectangulaire et renvoie "VRAI" (true) si le joueur clique dessus
bool DrawButton(Rectangle rect, Color bgColor, Color textColor, const char* text);

// Dessine une petite carte (radar) pour voir où sont les véhicules en global
void DrawMiniMap(const std::vector<Car*>& cars);

#endif